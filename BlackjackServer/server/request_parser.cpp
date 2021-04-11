#include "request_parser.hpp"
#include "request.hpp"

namespace http {
    namespace server {

        RequestParser::RequestParser()
            : state_(methodStart)
        {
        }

        void RequestParser::Reset()
        {
            state_ = methodStart;
        }

        RequestParser::ResultType RequestParser::Consume(Request& req, char input)
        {
            switch (state_)
            {
            case methodStart:
                if (!IsChar(input) || IsCtl(input) || IsTSpecial(input))
                {
                    return bad;
                }
                else
                {
                    state_ = method;
                    req.method.push_back(input);
                    return indeterminate;
                }
            case method:
                if (input == ' ')
                {
                    state_ = uri;
                    return indeterminate;
                }
                else if (!IsChar(input) || IsCtl(input) || IsTSpecial(input))
                {
                    return bad;
                }
                else
                {
                    req.method.push_back(input);
                    return indeterminate;
                }
            case uri:
                if (input == ' ')
                {
                    state_ = httpVersionH;
                    return indeterminate;
                }
                else if (IsCtl(input))
                {
                    return bad;
                }
                else
                {
                    req.uri.push_back(input);
                    return indeterminate;
                }
            case httpVersionH:
                if (input == 'H')
                {
                    state_ = httpVersionT1;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case httpVersionT1:
                if (input == 'T')
                {
                    state_ = httpVersionT2;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case httpVersionT2:
                if (input == 'T')
                {
                    state_ = httpVersionP;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case httpVersionP:
                if (input == 'P')
                {
                    state_ = httpVersionSlash;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case httpVersionSlash:
                if (input == '/')
                {
                    req.http_version_major = 0;
                    req.http_version_minor = 0;
                    state_ = httpVersionMajorStart;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case httpVersionMajorStart:
                if (IsDigit(input))
                {
                    req.http_version_major = req.http_version_major * 10 + input - '0';
                    state_ = httpVersionMajor;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case httpVersionMajor:
                if (input == '.')
                {
                    state_ = httpVersionMinorStart;
                    return indeterminate;
                }
                else if (IsDigit(input))
                {
                    req.http_version_major = req.http_version_major * 10 + input - '0';
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case httpVersionMinorStart:
                if (IsDigit(input))
                {
                    req.http_version_minor = req.http_version_minor * 10 + input - '0';
                    state_ = httpVersionMinor;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case httpVersionMinor:
                if (input == '\r')
                {
                    state_ = expectingNewline1;
                    return indeterminate;
                }
                else if (IsDigit(input))
                {
                    req.http_version_minor = req.http_version_minor * 10 + input - '0';
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case expectingNewline1:
                if (input == '\n')
                {
                    state_ = headerLineStart;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case headerLineStart:
                if (input == '\r')
                {
                    state_ = expectingNewline3;
                    return indeterminate;
                }
                else if (!req.headers.empty() && (input == ' ' || input == '\t'))
                {
                    state_ = headerLws;
                    return indeterminate;
                }
                else if (!IsChar(input) || IsCtl(input) || IsTSpecial(input))
                {
                    return bad;
                }
                else
                {
                    req.headers.push_back(Header());
                    req.headers.back().name.push_back(input);
                    state_ = headerName;
                    return indeterminate;
                }
            case headerLws:
                if (input == '\r')
                {
                    state_ = expectingNewline2;
                    return indeterminate;
                }
                else if (input == ' ' || input == '\t')
                {
                    return indeterminate;
                }
                else if (IsCtl(input))
                {
                    return bad;
                }
                else
                {
                    state_ = headerValue;
                    req.headers.back().value.push_back(input);
                    return indeterminate;
                }
            case headerName:
                if (input == ':')
                {
                    state_ = spaceBeforeHeaderValue;
                    return indeterminate;
                }
                else if (!IsChar(input) || IsCtl(input) || IsTSpecial(input))
                {
                    return bad;
                }
                else
                {
                    req.headers.back().name.push_back(input);
                    return indeterminate;
                }
            case spaceBeforeHeaderValue:
                if (input == ' ')
                {
                    state_ = headerValue;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case headerValue:
                if (input == '\r')
                {
                    state_ = expectingNewline2;
                    return indeterminate;
                }
                else if (IsCtl(input))
                {
                    return bad;
                }
                else
                {
                    req.headers.back().value.push_back(input);
                    return indeterminate;
                }
            case expectingNewline2:
                if (input == '\n')
                {
                    state_ = headerLineStart;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case expectingNewline3:
                return (input == '\n') ? good : bad;
            default:
                return bad;
            }
        }

        bool RequestParser::IsChar(int c)
        {
            return c >= 0 && c <= 127;
        }

        bool RequestParser::IsCtl(int c)
        {
            return (c >= 0 && c <= 31) || (c == 127);
        }

        bool RequestParser::IsTSpecial(int c)
        {
            switch (c)
            {
            case '(': case ')': case '<': case '>': case '@':
            case ',': case ';': case ':': case '\\': case '"':
            case '/': case '[': case ']': case '?': case '=':
            case '{': case '}': case ' ': case '\t':
                return true;
            default:
                return false;
            }
        }

        bool RequestParser::IsDigit(int c)
        {
            return c >= '0' && c <= '9';
        }

    } // namespace server
} // namespace http
