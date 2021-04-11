#pragma once

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

namespace http {
    namespace server {

        class ConnectionManager;

        /// Represents a single connection from a client.
        class Connection
            : public std::enable_shared_from_this<Connection>
        {
        public:
            Connection(const Connection&) = delete;
            Connection& operator=(const Connection&) = delete;

            /// Construct a connection with the given socket.
            explicit Connection(boost::asio::ip::tcp::socket socket,
                ConnectionManager& manager, RequestHandler& handler);

            /// Start the first asynchronous operation for the connection.
            void Start();

            /// Stop all asynchronous operations associated with the connection.
            void Stop();

        private:
            /// Perform an asynchronous read operation.
            void DoRead();

            /// Perform an asynchronous write operation.
            void DoWrite();

            /// Socket for the connection.
            boost::asio::ip::tcp::socket socket_;

            /// The manager for this connection.
            ConnectionManager& connection_manager_;

            /// The handler used to process the incoming request.
            RequestHandler& request_handler_;

            /// Buffer for incoming data.
            std::array<char, 8192> buffer_;

            /// The incoming request.
            Request request_;

            /// The parser for the incoming request.
            RequestParser request_parser_;

            /// The reply to be sent back to the client.
            Response reply_;
        };

        typedef std::shared_ptr<Connection> connection_ptr;

    } // namespace server
} // namespace http