#ifndef SINGLETON_H
#define SINGLETON_H

#include "type_traits.h"

namespace klib
{
    template <typename T>
    class singleton
    {
    public:
        template <typename... Args>
        static T &instance(Args&&... args)
        {
            static T instance(forward<Args>(args)...);
            return instance;
        }

        ~singleton()                           = delete;
        singleton(const singleton&)            = delete;
        singleton(singleton &&)                = delete;
        singleton &operator=(const singleton&) = delete;
        singleton &&operator=(singleton&&)     = delete;

    private:
        singleton()
        {
        }
    };
}

#endif // SINGLETON_H
