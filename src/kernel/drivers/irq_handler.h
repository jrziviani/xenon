#ifndef IRQ_HANDLER_H
#define IRQ_HANDLER_H

#include "list.h"
#include "type_traits.h"
#include <klib/logger.h>
#include <klib/xenon_base.h>

class irq_handler
{
    klib::list<xenon_base*> registered_;

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

    void trigger_ide()
    {
        for (auto obj : registered_) {
            obj->on_disk_event();
        }
    }
};

#endif // IRQ_HANDLER_H
