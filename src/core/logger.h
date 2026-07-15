#pragma once

#include <format>
#include <iostream>

// Temporary logger intended for debugging during development.

namespace dawn::core
{
    class Logger
    {
      public:
        Logger() = delete;

        template <typename... Args>
        static void Debug(std::format_string<Args...> fmt, Args&&... args)
        {

#if defined(_DEBUG) || defined(DEBUG)
            const std::string msg = std::format(fmt, std::forward<Args>(args)...);

            std::cout << "[DEBUG]: " << msg << std::endl;
#endif
        }
    };
} // namespace dawn::core