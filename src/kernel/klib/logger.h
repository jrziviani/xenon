#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <klib/stdint.h>
#include <klib/stdlib.h>
#include <klib/string.h>
#include <klib/singleton.h>

// TODO: this dep shouldn't exist...
#include <arch/amd64/video/vga_terminal.h>

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
            term_.clear();
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
                    unsigned int width = 0;
                    c = *++format;

                    while (isdigit(c)) {
                        width *= 10;
                        width += c - '0';
                        c = *++format;
                    }

                    switch (c) {
                        case 'd':
                            itoa(buffer, 64, va_arg(args, int64_t));
                            term_.prints(buffer, '0', width);
                            break;

                        case 'x':
                            itoa(buffer, 64, va_arg(args, int64_t), base::hex);
                            term_.prints(buffer, '0', width);
                            break;

                        case 'b':
                            itoa(buffer, 64, va_arg(args, int64_t), base::bin);
                            term_.prints(buffer, '0', width);
                            break;

                        case 's':
                            term_.prints(va_arg(args, const char*), ' ', width);
                            break;

                        case 'c':
                            term_.printc(static_cast<char>(va_arg(args, int)));
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
