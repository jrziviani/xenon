#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "char_device.h"

namespace xenon
{
    enum class keycode_t;

    class keyboard : public char_device
    {

        static const char *keycode_to_str(keycode_t key);
    };
};

#endif // KEYBOARD_H
