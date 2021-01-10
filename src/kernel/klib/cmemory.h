#ifndef CMEMORY_H
#define CMEMORY_H

#include "utility.h"
#include "stdint.h"
#include "type_traits.h"

namespace klib
{
    template <typename T>
    struct default_delete
    {
        constexpr default_delete() noexcept = default;

        void operator()(T *ptr) const
        {
            delete ptr;
        }
    };

    template <typename T>
    struct default_delete<T[]>
    {
        constexpr default_delete() noexcept = default;

        template<typename U>
        void operator()(U *ptr) const
        {
            delete[] ptr;
        }
    };

    template <typename T, typename D = default_delete<T>>
    class unique_ptr
    {
        T *data_;
        D deleter_;

    public:
        using element_type = T;

    public:
        constexpr unique_ptr() noexcept :
            data_(nullptr)
        {
        }

        constexpr unique_ptr(nullptr_t) noexcept :
            data_(nullptr)
        {
        }

        explicit unique_ptr(T *data) :
            data_(data)
        {
        }

        template <typename Deleter>
        unique_ptr(T *data, Deleter &&del) :
            data_(data),
            deleter_(forward<Deleter>(del))
        {
        }

        unique_ptr(unique_ptr &&that) noexcept :
            data_(move(that.data_))
        {
        }

        unique_ptr &operator=(unique_ptr &&that) noexcept
        {
            reset(that.release());
            deleter_ = forward<D>(that.deleter_);
            return *this;
        }

        unique_ptr &operator=(nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        ~unique_ptr() noexcept
        {
            if (data_ != nullptr) {
                deleter_(data_);
            }

            data_ = nullptr;
        }

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr &operator=(const unique_ptr&) = delete;

        explicit operator bool()
        {
            return (data_ == nullptr) ? false : true;
        }

        T *get() const noexcept
        {
            return data_;
        }

        T *operator->() const noexcept
        {
            return data_;
        }

        T &operator*() const
        {
            return *data_;
        }

        T *release() noexcept
        {
            T *result = nullptr;
            swap(result, data_);
            return result;
        }

        void reset(T *obj) noexcept
        {
            const T *old = nullptr;
            swap(old, data_);
            swap(data_, obj);

            if (old) {
                deleter_(old);
            }
        }

        void swap(unique_ptr &rhs) noexcept
        {
            swap(data_, rhs.data);
            swap(deleter_, rhs.deleter_);
        }
    };

    template <typename T, typename D>
    class unique_ptr<T[], D>
    {
        T *data_;
        D deleter_;

    public:
        constexpr unique_ptr() noexcept :
            data_(nullptr)
        {
        }

        constexpr unique_ptr(nullptr_t) noexcept :
            data_(nullptr)
        {
        }

        explicit unique_ptr(T *data) :
            data_(data)
        {
        }

        template <typename Deleter>
        unique_ptr(T *data, Deleter &&del) :
            data_(data),
            deleter_(forward<Deleter>(del))
        {
        }

        unique_ptr(unique_ptr &&that) noexcept :
            data_(move(that.data_))
        {
        }

        unique_ptr &operator=(unique_ptr &&that) noexcept
        {
            reset(that.release());
            deleter_ = forward<D>(that.deleter_);
            return *this;
        }

        unique_ptr &operator=(nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        ~unique_ptr() noexcept
        {
            if (data_ != nullptr) {
                deleter_(data_);
            }

            data_ = nullptr;
        }

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr &operator=(const unique_ptr&) = delete;

        explicit operator bool()
        {
            return (data_ == nullptr) ? false : true;
        }

        T *get() const noexcept
        {
            return data_;
        }

        T *operator->() const noexcept
        {
            return data_;
        }

        T &operator*() const
        {
            return *data_;
        }

        T operator[](size_t i) const
        {
            return data_[i];
        }

        T &operator[](size_t i)
        {
            return data_[i];
        }

        T *release() noexcept
        {
            T *result = nullptr;
            klib::swap(result, data_);
            return result;
        }

        void reset(T *obj) noexcept
        {
            T *old = nullptr;
            klib::swap(old, data_);
            klib::swap(data_, obj);

            if (old) {
                deleter_(old);
            }
        }

        void swap(unique_ptr &rhs) noexcept
        {
            klib::swap(data_, rhs.data);
            klib::swap(deleter_, rhs.deleter_);
        }
    };

    template <class T, class... Args>
    unique_ptr<T> make_unique(Args&&... args)
    {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }

    template <class T>
    unique_ptr<T> make_unique(size_t size)
    {
        return unique_ptr<T>(new remove_extent_t<T>[size]());
    }
}

#endif // CMEMORY_H
