#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdarg.h>

#include "vga_terminal.h"
#include "singleton.h"

namespace xenon
{
    enum class logger_output : uint8_t
    {
        VGA,
        SOCKET,
        SERIAL,
        FILE,
    };

    class logger__
    {
        friend class singleton<logger__>;

        logger_output out_;
        vga_terminal term_;

        logger__()
        {
            out_ = logger_output::VGA;
        }

        ~logger__()
        {
        }

    public:
        logger__(const logger__&) = delete;
        logger__(logger__&&) = delete;
        logger__ &operator=(const logger__&) = delete;
        logger__ &&operator=(logger__&&) = delete;

        int log(const char *format, ...)
        {
            va_list args;

            va_start(args, format);
            term_.printf(format, args);
            term_.printf("\n");
            va_end(args);

            return 0;
        }
    };

    using logger = singleton<logger__>;
}

#endif // LOGGER
