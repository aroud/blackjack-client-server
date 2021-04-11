#include "mime_types.hpp"

namespace http {
    namespace server {
        namespace mime_types {

            struct Mapping
            {
                const char* extension;
                const char* mime_type;
            } mappings[] =
            {
              { "gif", "image/gif" },
              { "htm", "text/html" },
              { "html", "text/html" },
              { "jpg", "image/jpeg" },
              { "png", "image/png" }
            };

            std::string ExtensionToType(const std::string& extension)
            {
                for (Mapping m : mappings)
                {
                    if (m.extension == extension)
                    {
                        return m.mime_type;
                    }
                }

                return "text/plain";
            }

        } // namespace mime_types
    } // namespace server
} // namespace http
