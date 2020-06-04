#ifndef IRQ_HANDLER_H
#define IRQ_HANDLER_H

#include "llist.h"
#include "type_traits.h"

namespace xenon
{
    template <class C>
    class irq_handler
    {
        llist<C*> registered_;

        reflection_has_member(on_time);
        reflection_has_member(on_keyup);
        reflection_has_member(on_keydown);

    public:
        virtual ~irq_handler()
        {
            registered_.clear();
        }

        void register_me(C *obj)
        {
            registered_.push_back(obj);
        }

        void trigger_timer()
        {
            if constexpr (has_member_on_time<C>::value) {
                for (auto obj : registered_) {
                    obj->on_time();
                }
            }
        }
    };
}

#endif // IRQ_HANDLER_H
