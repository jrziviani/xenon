#ifndef LLIST_H
#define LLIST_H

#include <memory/allocators.h>
#include "new.h"

namespace xenon
{
    template <typename T, class Alloc=basic_allocator>
    class llist
    {
        struct node
        {
            T data_;
            node *next_;
            node *previous_;

            node() :
                next_(nullptr),
                previous_(nullptr)
            {
            }

            node(T data) :
                data_(data),
                next_(nullptr),
                previous_(nullptr)
            {
            }
        };

        class iterator
        {
            node *current_;

        public:
            iterator(node *n) :
                current_(n)
            {
            }

            iterator &operator++()
            {
                if (current_ != nullptr) {
                    current_ = current_->next_;
                }

                return *this;
            }

            iterator operator++(int)
            {
                iterator tmp = *this;
                ++*this;
                return tmp;
            }

            iterator &operator=(node *n)
            {
                current_ = n;
                return *this;
            }

            bool operator!=(const iterator &it) const
            {
                return current_ != it.current_;
            }

            T operator*() const
            {
                if (current_ == nullptr) {
                    /*
                    T test;
                    return test;
                    */
                }

                return current_->data_;
            }
        };

    private:
        node *head_;
        node *tail_;
        size_t size_;

        Alloc allocator_;

    public:
        using const_iterator = const iterator;

        llist() :
            head_(nullptr),
            tail_(nullptr)
        {
        }

        ~llist()
        {
            clear();
        }

        size_t size() const
        {
            return size_;
        }

        bool empty() const
        {
            return size_ == 0;
        }

        void push_back(T data)
        {
            if (tail_ == nullptr) {
                tail_ = allocator_.template create<node>(data);
                head_ = tail_;
            }
            else {
                tail_->next_ = allocator_.template create<node>(data);
                tail_ = tail_->next_;
            }

            ++size_;
        }

        void push_front(T data)
        {
            node *n = allocator_.template create<node>(data);
            n->next_ = head_;
            head_ = n;

            if (tail_ == nullptr) {
                tail_ = head_;
            }

            ++size_;
        }

        T pop_back()
        {
            T data = tail_->data_;

            delete tail_;
            tail_ = tail_->previous_;

            size_--;

            return data;
        }

        T pop_front()
        {
            T data = head_->data_;

            delete head_;
            head_ = head_->next_;

            size_--;

            return data;
        }

        void remove(T data)
        {
            node *n;
            for (n = head_; n != nullptr && n->data_ != data; n = n->next_) ;

            if (n == nullptr) {
                return;
            }

            if (n->previous_ != nullptr) {
                n->previous_->next_ = n->next_;
            }

            if (n->next_ != nullptr) {
                n->next_->previous_ = n->previous_;
            }

            delete n;
            n = nullptr;
            size_--;

            if (size_ == 0) {
                head_ = tail_ = nullptr;
            }
        }

        void clear()
        {
            int total = 0;
            for (node *n = head_; n != nullptr; n = n->next_) {
                allocator_.destroy(n);
                ++total;
            }

            head_ = tail_ = nullptr;
            size_ = 0;
        }

        const_iterator begin() const
        {
            return iterator(head_);
        }

        const_iterator end() const
        {
            return iterator(nullptr);
        }
    };
}

#endif // LLIST_H
