#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "char_device.h"

namespace xenon
{
    enum class keycode_t;

    class keyboard : public char_device
    {
    public:
        void on_keyup(char c);
        void on_keydown(char c);

    public:
        virtual void on_keyboard()
        {
        }
    };
};

#endif // KEYBOARD_H
