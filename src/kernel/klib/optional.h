#ifndef OPTIONAL_H
#define OPTIONAL_H

namespace klib
{
    template <typename T>
    class optional
    {
        T data_;
        bool assigned_;

    public:
        optional() :
            assigned_(false)
        {
        }

        optional(T data) :
            data_(data),
            assigned_(true)
        {
        }

        /*
        explicit operator bool()
        {
            return assigned_;
        }*/
    };
}

#endif // OPTIONAL_H
