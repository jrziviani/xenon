#ifndef LLIST_H
#define LLIST_H

#include <memory/allocators.h>
#include "new.h"

namespace xenon
{
    template <typename T>
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

    public:
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

        void push_back(T data)
        {
            void *place = placement_kalloc(sizeof(node));
            if (tail_ == nullptr) {
                tail_ = new (place) node(data);
                head_ = tail_;
            }
            else {
                tail_->next_ = new (place) node(data);
                tail_ = tail_->next_;
            }

            ++size_;
        }

        void push_front(T data)
        {
            void *place = placement_kalloc(sizeof(node));
            node *n = new (place) node(data);
            n->next_ = head_;
            head_ = n;

            if (tail_ == nullptr) {
                tail_ = head_;
            }

            ++size_;
        }

        void clear()
        {
            int total = 0;
            for (node *n = head_; n != nullptr; n = n->next_) {
                ++total;
            }

            head_ = tail_ = nullptr;
            kfree_block(sizeof(node) * total);
            size_ = 0;
        }

        iterator begin() const
        {
            return iterator(head_);
        }

        iterator end() const
        {
            return iterator(nullptr);
        }
    };
}

#endif // LLIST_H
