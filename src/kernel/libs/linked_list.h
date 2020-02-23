#ifndef LINKED_LIST_H
#define LINKED_LIST_H

namespace xenon
{
    template <typename T>
    struct node
    {
        T data_;

        node<T> *next_;
        node<T> *previous_;

        node(T data) :
            data_(data),
            next_(nullptr),
            previous_(nullptr)
        {
        }
    };

    template <typename T>
    class linked_list
    {
        node<T> *head_;
        node<T> *tail_;

    public:
        void push_front(T data)
        {
            node<T> *blk = new node<T>(data);
            blk->next_ = head_;
            head_ = blk;
        }

        void push_back(T data)
        {
            node<T> *blk = new node<T>(data);
            blk->previous_ = tail_;
            tail_ = blk;
        }

        node<T> *pop_front()
        {
            node<T> *front = head_;
            head_ = front->next_;
            return front;
        }

        node<T> *pop_back()
        {
            node<T> *back = tail_;
            tail_ = back->previous_;
            return back;
        }

        T front_or(T ret)
        {
            if (head_ == nullptr) {
                return ret;
            }

            return head_->data_;
        }

        T back_or(T ret)
        {
            if (tail_ == nullptr) {
                return ret;
            }

            return tail_->data_;
        }

        template <typename F>
        T find_or(F &&fn, T ret)
        {
            for (node<T> *iter = head_; iter != nullptr; iter = iter->next_) {
                if (fn(iter->data_)) {
                    return iter->data_;
                }
            }

            return ret;
        }
    };
}

#endif // LINKED_LIST_H
