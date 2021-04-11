#pragma once

#include <tuple>

namespace http {
    namespace server {

        struct Request;

        /// Parser for incoming requests.
        class RequestParser
        {
        public:
            /// Construct ready to parse the request method.
            RequestParser();

            /// Reset to initial parser state.
            void Reset();

            /// Result of parse.
            enum ResultType { good, bad, indeterminate };

            /// Parse some data. The enum return value is good when a complete request has
            /// been parsed, bad if the data is invalid, indeterminate when more data is
            /// required. The InputIterator return value indicates how much of the input
            /// has been consumed.
            template <typename InputIterator>
            std::tuple<ResultType, InputIterator> Parse(Request& req,
                InputIterator begin, InputIterator end)
            {
                while (begin != end)
                {
                    ResultType result = Consume(req, *begin++);
                    if (result == good || result == bad)
                        return std::make_tuple(result, begin);
                }
                return std::make_tuple(indeterminate, begin);
            }

        private:
            /// Handle the next character of input.
            ResultType Consume(Request& req, char input);

            /// Check if a byte is an HTTP character.
            static bool IsChar(int c);

            /// Check if a byte is an HTTP control character.
            static bool IsCtl(int c);

            /// Check if a byte is defined as an HTTP tspecial character.
            static bool IsTSpecial(int c);

            /// Check if a byte is a digit.
            static bool IsDigit(int c);

            /// The current state of the parser.
            enum state
            {
                methodStart,
                method,
                uri,
                httpVersionH,
                httpVersionT1,
                httpVersionT2,
                httpVersionP,
                httpVersionSlash,
                httpVersionMajorStart,
                httpVersionMajor,
                httpVersionMinorStart,
                httpVersionMinor,
                expectingNewline1,
                headerLineStart,
                headerLws,
                headerName,
                spaceBeforeHeaderValue,
                headerValue,
                expectingNewline2,
                expectingNewline3
            } state_;
        };

    } // namespace server
} // namespace http
