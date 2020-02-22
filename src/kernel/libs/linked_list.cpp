#include "linked_list.h"

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
    void linked_list<T>::push_front(node<T> *data)
    {
        data->next_ = head_;
        data->previous_ = nullptr;
        head_ = data;
    }

    template <typename T>
    void linked_list<T>::push_back(node<T> *data)
    {
        data->next_ = nullptr;
        data->previous_ = tail_;
        tail_ = data;
    }

    template <typename T>
    node<T> *linked_list<T>::pop_front()
    {
        node<T> *front = head_;
        head_ = front->next_;
        return front;
    }

    template <typename T>
    node<T> *linked_list<T>::pop_back()
    {
        node<T> *back = tail_;
        tail_ = back->previous_;
        return back;
    }
}
