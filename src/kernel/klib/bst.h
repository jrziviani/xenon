#ifndef BST_H
#define BST_H

#include <memory/allocators.h>

namespace klib
{
    template <typename T>
    class bst
    {
        struct node
        {
            T data;
            node *left;
            node *right;

            node() :
                left(nullptr),
                right(nullptr)
            {
            }

            node(T d) :
                data(d),
                left(nullptr),
                right(nullptr)
            {
            }
        };

    private:
        node *head_;

    private:
        node **get_spot(node **parent, T data)
        {
            node *rparent = *parent;

            if (rparent == nullptr) {
                return parent;
            }

            if (data < rparent->data) {
                return get_spot(&rparent->left, data);
            }
            else {
                return get_spot(&rparent->right, data);
            }
        }

        node **find(node **parent, T data)
        {
            node *rparent = *parent;

            if (rparent == nullptr) {
                return nullptr;
            }

            if (rparent->data == data) {
                return parent;
            }
            else if (rparent->data < data) {
                return find(&rparent->left, data);
            }
            else {
                return find(&rparent->right, data);
            }
        }

        node **find_smaller(node **parent)
        {
            if (*parent == nullptr) {
                return parent;
            }

            find_smaller(&(*parent)->left);
        }

        template <class L>
        void foreach(node *parent, L &lambda)
        {
            if (parent == nullptr) {
                return;
            }

            foreach(parent->left, lambda);
            if (lambda(parent->data)) {
                return;
            }
            foreach(parent->right, lambda);
        }

    public:
        void insert(T data)
        {
            void *place = placement_kalloc(sizeof(node));
            node *n = new (place) node(data);

            auto spot = get_spot(&head_, data);
            *spot = n;
        }

        void remove(T data)
        {
            auto to_replace = find(&head_, data);
            if (to_replace == nullptr) {
                return;
            }

            auto to_remove = find_smaller((*to_replace)->right);

            (*to_replace)->data = (*to_remove)->data;
            *to_remove = nullptr;
            kfree_block(sizeof(node));
        }

        template <class L>
        void foreach(L &lambda)
        {
            foreach(head_, lambda);
        }
    };
}

#endif // BST_H
