#include "connection.hpp"
#include <utility>
#include <vector>
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
    namespace server {

        Connection::Connection(boost::asio::ip::tcp::socket socket,
            ConnectionManager& manager, RequestHandler& handler)
            : socket_(std::move(socket)),
            connection_manager_(manager),
            request_handler_(handler)
        {
        }

        void Connection::Start()
        {
            DoRead();
        }

        void Connection::Stop()
        {
            socket_.close();
        }

        void Connection::DoRead()
        {
            auto self(shared_from_this());
            socket_.async_read_some(boost::asio::buffer(buffer_),
                [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
                {
                    if (!ec)
                    {
                        RequestParser::ResultType result;
                        std::tie(result, std::ignore) = request_parser_.Parse(
                            request_, buffer_.data(), buffer_.data() + bytes_transferred);

                        if (result == RequestParser::good)
                        {
                            request_handler_.HandleRequest(request_, reply_);
                            DoWrite();
                        }
                        else if (result == RequestParser::bad)
                        {
                            reply_ = Response::StockReply(Response::bad_request);
                            DoWrite();
                        }
                        else
                        {
                            DoRead();
                        }
                    }
                    else if (ec != boost::asio::error::operation_aborted)
                    {
                        connection_manager_.Stop(shared_from_this());
                    }
                });
        }

        void Connection::DoWrite()
        {
            auto self(shared_from_this());
            boost::asio::async_write(socket_, reply_.ToBuffers(),
                [this, self](boost::system::error_code ec, std::size_t)
                {
                    if (!ec)
                    {
                        // Initiate graceful connection closure.
                        boost::system::error_code ignored_ec;
                        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                            ignored_ec);
                    }

                    if (ec != boost::asio::error::operation_aborted)
                    {
                        connection_manager_.Stop(shared_from_this());
                    }
                });
        }

    } // namespace server
} // namespace http
