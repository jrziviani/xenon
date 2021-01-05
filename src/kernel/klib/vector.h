#ifndef VECTOR_H
#define VECTOR_H

#include "cmemory.h"
#include "string.h"
#include "optional.h"

namespace klib
{
    constexpr size_t DEFAULT_CAPACITY = 1;

    template <typename T>
    class vector
    {
        unique_ptr<T[]> arr_;
        size_t size_;
        size_t capacity_;

        class iterator
        {
            size_t index_;
            const T *const data_;

        public:
            iterator(const T *const data, size_t i) :
                index_(i),
                data_(data)
            {
            }

            iterator &operator++()
            {
                ++index_;
                return *this;
            }

            iterator operator++(int)
            {
                iterator tmp = *this;
                ++*this;
                return tmp;
            }

            iterator &operator=(size_t i)
            {
                index_ = i;
                return *this;
            }

            bool operator!=(const iterator &it) const
            {
                return index_ != it.index_;
            }

            T operator*() const
            {
                return data_[index_];
            }
        };

        using const_iterator = const iterator;

    private:
        void doubleme()
        {
            // assert(capacity_ > 0);
            auto tmp = make_unique<T[]>(capacity_ * 2);
            memcpy(tmp.get(), arr_.get(), sizeof(T) * size_);
            arr_ = move(tmp);
            capacity_ *= 2;
        }

        void halfme()
        {
            // assert(capacity_ > 0);
            auto tmp = make_unique<T[]>(capacity_ / 2);
            memcpy(tmp.get(), arr_.get(), sizeof(T) * size_);
            arr_ = move(tmp);
            capacity_ /= 2;
        }

    public:
        vector() :
            vector(DEFAULT_CAPACITY)
        {
        }

        vector(size_t capacity) :
            arr_(make_unique<T[]>(capacity)),
            size_(0),
            capacity_(capacity)
        {
        }

        size_t size() const
        {
            return size_;
        }

        size_t capacity() const
        {
            return capacity_;
        }

        bool empty() const
        {
            return size_ == 0;
        }

        void push_back(T value)
        {
            if (size_ == capacity_) {
                doubleme();
            }

            arr_[size_++] = value;
        }

        optional<T> pop_back()
        {
            if (size_ == 0) {
                return optional<T>();
            }

            T ret = arr_[--size_];
            if (size_ <= capacity_ / 4) {
                halfme();
            }

            return ret;
        }

        optional<T> back() const
        {
            if (size_ == 0) {
                return optional<T>();
            }

            return arr_[size_ - 1];
        }

        optional<T> at(size_t index) const
        {
            if (index >= size_) {
                return optional<T>();
            }

            return arr_[index];
        }

        const_iterator begin() const
        {
            return iterator(arr_.get(), 0);
        }

        const_iterator end() const
        {
            return iterator(arr_.get(), size_);
        }
    };
}

#endif // VECTOR_H
