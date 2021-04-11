#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "header.hpp"

namespace http {
    namespace server {

        /// A reply to be sent to a client.
        struct Response
        {
            /// The status of the reply.
            enum StatusType
            {
                ok = 200,
                created = 201,
                accepted = 202,
                no_content = 204,
                multiple_choices = 300,
                moved_permanently = 301,
                moved_temporarily = 302,
                not_modified = 304,
                bad_request = 400,
                unauthorized = 401,
                forbidden = 403,
                not_found = 404,
                internal_server_error = 500,
                not_implemented = 501,
                bad_gateway = 502,
                service_unavailable = 503
            } status;

            /// The headers to be included in the reply.
            std::vector<Header> headers;

            /// The content to be sent in the reply.
            std::string content;

            /// Convert the reply into a vector of buffers. The buffers do not own the
            /// underlying memory blocks, therefore the reply object must remain valid and
            /// not be changed until the write operation has completed.
            std::vector<boost::asio::const_buffer> ToBuffers();

            /// Get a stock reply.
            static Response StockReply(StatusType status);
        };

    } // namespace server
} // namespace http