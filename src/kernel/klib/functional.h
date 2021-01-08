#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include "type_traits.h"

namespace klib
{
    template <typename T>
    concept has_run = requires(T t)
    {
        t.run();
    };

    template <typename T>
    class function_st : public T
    {
        T impl_;

    public:
        template <typename... Args>
        void run(Args&&... args)
        {
            impl_.run(args...);
        }

        ~function_st()                             = delete;
        function_st(const function_st&)            = delete;
        function_st(function_st &&)                = delete;
        function_st &operator=(const function_st&) = delete;
        function_st &&operator=(function_st&&)     = delete;
    };

    class callback;
    using function = function_st<callback>;
}

#endif // FUNCTIONAL_H
