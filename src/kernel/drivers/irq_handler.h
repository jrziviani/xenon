#ifndef IRQ_HANDLER_H
#define IRQ_HANDLER_H

#include "llist.h"
#include "type_traits.h"
#include <klib/logger.h>
#include <klib/xenon_base.h>

namespace xenon
{
    class irq_handler
    {
        llist<xenon_base*> registered_;

        reflection_has_member(on_time);
        reflection_has_member(on_keyup);
        reflection_has_member(on_keydown);

    public:
        virtual ~irq_handler()
        {
            registered_.clear();
        }

        template <typename T>
        void register_me(xenon_base *obj)
        {
            registered_.push_back(obj);
        }

        void trigger_timer()
        {
            for (auto obj : registered_) {
                obj->on_time();
            }
        }

        void trigger_keyboard()
        {
            for (auto obj : registered_) {
                obj->on_keyboard();
            }
        }
    };
}

#endif // IRQ_HANDLER_H
