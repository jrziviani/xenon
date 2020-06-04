#include "amd64_keyboard.h"

#include <klib/logger.h>

constexpr uint16_t DATA_IN_BUFFER = 0x01;
constexpr uint16_t STATUS_PORT = 0x64;
constexpr uint16_t DATA_PORT = 0x60;

namespace xenon
{
    void amd64_keyboard::on_keyboard()
    {
        auto status = inb(STATUS_PORT);
        if ((status & DATA_IN_BUFFER) == 0) {
            return;
        }

        auto data = inb(DATA_PORT);
        logger::instance().log("KEY PRESSED: %c", data);
    }
}
