#ifndef LINKED_LIST_H
#define LINKED_LIST_H

namespace xenon
{
    template <typename T>
    struct node;

    template <typename T>
    class linked_list
    {
        node<T> *head_;
        node<T> *tail_;

    public:
        void push_front(node<T> *data);
        void push_back(node<T> *data);

        node<T> *pop_front();
        node<T> *pop_back();
    };
}

#endif // LINKED_LIST_H
