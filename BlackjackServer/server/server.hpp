#pragma once

#include <boost/asio.hpp>
#include <string>
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
    namespace server {

        /// The top-level class of the HTTP server.
        class Server
        {
        public:
            Server(const Server&) = delete;
            Server& operator=(const Server&) = delete;

            /// Construct the server to listen on the specified TCP address and port, and
            /// serve up files from the given directory.
            explicit Server(const std::string& address, const std::string& port,
                const std::string& doc_root);

            /// Run the server's io_context loop.
            void Run();

        private:
            /// Perform an asynchronous accept operation.
            void DoAccept();

            /// Wait for a request to stop the server.
            void DoAwaitStop();

            /// The io_context used to perform asynchronous operations.
            boost::asio::io_context io_context_;

            /// The signal_set is used to register for process termination notifications.
            boost::asio::signal_set signals_;

            /// Acceptor used to listen for incoming connections.
            boost::asio::ip::tcp::acceptor acceptor_;

            /// The connection manager which owns all live connections.
            ConnectionManager connection_manager_;

            /// The handler for all incoming requests.
            RequestHandler request_handler_;
        };

    } // namespace server
} // namespace http

