#ifndef VGA_TERMINAL_H
#define VGA_TERMINAL_H

#include <klib/stdint.h>

enum colors
{
    BLACK = 0,
    BLUE,
    GREEN,
    CYAN,
    RED,
    PURPLE,
    BROWN,
    GRAY,
    DARK_GRAY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_PURPLE,
    YELLOW,
    WHITE
};

constexpr size_t WIDTH = 80;
constexpr size_t HEIGHT = 25;

class vga_terminal
{
    colors current_color_;
    uint8_t col_;
    uint8_t lin_;

    int16_t buffer_[WIDTH * (HEIGHT * 2)];

private:
    void scroll_down();

public:
    vga_terminal();

    void set_color(colors color = colors::GRAY);

    void clear();
    void printc(char c);
    void prints(const char *s);
    void prints(const char *s, char fill, unsigned int times);
    int printf(const char *format, ...);

    void column(uint8_t col)
    {
        col_ = (col > WIDTH) ? 0 : col;
    }

    uint8_t column()
    {
        return col_;
    }

    void line(uint8_t lin)
    {
        lin_ = (lin > HEIGHT) ? 0 : lin;
    }

    uint8_t line()
    {
        return lin_;
    }
};

#endif // VGA_TERMINAL_H
