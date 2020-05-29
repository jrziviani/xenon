#include "heap.h"
#include "allocators.h"

#include <klib/new.h>

const int THRSLD = 512;

namespace xenon
{
    heap::heap() :
        start_(nullptr),
        size_(0)
    {
    }

    heap::heap(vaddr_t start, size_t size) :
        start_(start),
        size_(size)
    {
    }

    void heap::setup(vaddr_t start, size_t size)
    {
        start_ = start;
        size_ = size;
    }

    heap::node *heap::expand(size_t size)
    {
        node *n = new (start_) node(start_, size);
        free_heap_.insert(n);

        start_ = static_cast<char*>(start_) + size + sizeof(node);
        size_ -= size - sizeof(node);

        return n;
    }

    vaddr_t heap::alloc(size_t size)
    {
        if (size > size_) {
            return nullptr;
        }

        vaddr_t addr = nullptr;
        auto best_slot = [&addr, size](node *n) -> bool {
            if (size > n->size || n->free == false) {
                return false;
            }

            if (size + THRSLD < n->size) {
                // TODO: create a new block then size requested is too
                // small.
            }

            n->free = false;
            addr = n->addr;

            return true;
        };

        // TODO: BUG HERE
        //free_heap_.foreach(best_slot);

        if (addr == nullptr) {
            node *n = expand(size);
            n->free = false;
            addr = n->addr;
        }

        return addr;
    }

    void heap::dealloc(vaddr_t addr)
    {
        if (addr == nullptr) {
            // PANIC
        }

        node *n = static_cast<node*>(addr) - 16;
        n->free = true;
    }
}
