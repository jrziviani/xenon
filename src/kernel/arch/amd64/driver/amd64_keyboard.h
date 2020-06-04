#ifndef AMD64_KEYBOARD_H
#define AMD64_KEYBOARD_H

#include <drivers/keyboard.h>
#include <arch/amd64/instructions.h>

namespace xenon
{
    class amd64_keyboard : public keyboard
    {
    public:
        virtual void on_keyboard() final;
    };
}

#endif // AMD64_KEYBOARD_H
