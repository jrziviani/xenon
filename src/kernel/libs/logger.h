#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdlib.h>
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
            char buffer[64] = {0};
            va_list args;
            va_start(args, format);

            for (char c = *format; c != '\0'; c = *++format) {
                if (c != '%') {
                    term_.printc(c);
                }
                else {
                    c = *++format;
                    switch (c) {
                        case 'd':
                            itoa(buffer, 64, va_arg(args, int64_t));
                            term_.prints(buffer);
                            break;

                        case 'x':
                            itoa(buffer, 64, va_arg(args, int64_t), base::hex);
                            term_.prints(buffer);
                            break;

                        case 'b':
                            itoa(buffer, 64, va_arg(args, int64_t), base::bin);
                            term_.prints(buffer);
                            break;

                        case 's':
                            term_.prints(va_arg(args, const char*));
                            break;

                        default:
                            term_.printc(c);
                            break;
                    }
                }
            }
            va_end(args);
            term_.prints("\n");

            return 0;
        }
    };

    using logger = singleton<logger__>;
}

#endif // LOGGER
