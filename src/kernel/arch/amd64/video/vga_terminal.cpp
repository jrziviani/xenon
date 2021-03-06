#include "vga_terminal.h"
#include "config.h"

#include <stdarg.h>
#include <klib/stdlib.h>
#include <klib/string.h>

vga_terminal::vga_terminal() :
    current_color_(colors::GRAY),
    col_(0),
    lin_(0)
{
}

void vga_terminal::scroll_down()
{
    auto videobuf = reinterpret_cast<uint16_t*>(VGA_VIRTUAL_ADDRESS);
    for (size_t y = 1; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            videobuf[(y - 1) * WIDTH + x] = videobuf[y * WIDTH + x];
        }
    }

    for (size_t x = 0; x < WIDTH; x++) {
        videobuf[(HEIGHT - 1) * WIDTH + x] = ' ' | 1 << 8;
    }

    lin_ = HEIGHT - 1;
}

void vga_terminal::set_color(colors color)
{
    current_color_ = color;
}

void vga_terminal::clear()
{
    current_color_ = colors::GRAY;

    auto videobuf = reinterpret_cast<uint16_t*>(VGA_VIRTUAL_ADDRESS);
    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            videobuf[y * WIDTH + x] = ' ' | 1 << 8;
        }
    }

    lin_ = 0;
    col_ = 0;
}

void vga_terminal::printc(char c)
{
    auto videobuf = reinterpret_cast<uint16_t*>(VGA_VIRTUAL_ADDRESS);

    if (c == '\n' || c == '\r') {
        col_ = 0;
        lin_++;
        if (lin_ >= HEIGHT) {
            scroll_down();
        }
        return;
    }

    videobuf[lin_ * WIDTH + col_] = c | current_color_ << 8;
    col_++;
    if (col_ >= WIDTH) {
        col_ = 0;
        lin_++;
        if (lin_ >= HEIGHT) {
            scroll_down();
        }
    }
}

void vga_terminal::prints(const char *s)
{
    prints(s, ' ', 0);
}

void vga_terminal::prints(const char *s, char fill, unsigned int times)
{
    for (auto len = klib::strlen(s); len < times; len++) {
        printc(fill);
    }

    while (*s != '\0') {
        switch (*s) {
            case '\n':
                lin_++;
                col_ = 0;
                if (lin_ >= HEIGHT) {
                    scroll_down();
                }
                break;

            case '\t':
                col_ += 4 - (col_ % 4);
                if (col_ >= WIDTH) {
                    lin_++;
                    col_ = 0;
                    if (lin_ > HEIGHT) {
                        scroll_down();
                    }
                }
                break;

            default:
                printc(*s);
                break;
        }
        s++;
    }
}

int vga_terminal::printf(const char *format, ...)
{
    char buffer[64] = {0};
    va_list args;
    va_start(args, format);

    for (char c = *format; c != '\0'; c = *++format) {
        if (c != '%') {
            printc(c);
        }
        else {
            c = *++format;
            unsigned int zeroes = 0;

            while (klib::isdigit(c)) {
                zeroes *= 10;
                zeroes += c - '0';
                c = *++format;
            }

            switch (c) {
                case 'd':
                    klib::itoa(buffer, 64, va_arg(args, int64_t));
                    prints(buffer);
                    break;

                case 'x':
                    klib::itoa(buffer, 64, va_arg(args, int64_t), klib::base::hex);
                    prints(buffer);
                    break;

                case 'b':
                    klib::itoa(buffer, 64, va_arg(args, int64_t), klib::base::bin);
                    prints(buffer);
                    break;

                case 's':
                    prints(va_arg(args, const char*));
                    break;

                default:
                    printc(c);
                    break;
            }
        }
    }
    va_end(args);

    return 0;
}
