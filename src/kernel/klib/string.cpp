#include "string.h"

namespace klib
{
    string::string(const char *str) :
        capacity_(strlen(str)),
        size_(capacity_),
        data_(make_unique<char[]>(capacity_ + 1))
    {
        memcpy(data_.get(), str, size_);
        data_[size_] = '\0';
    }

    string::string(const char *str, size_t size) :
        capacity_(size),
        size_(size),
        data_(make_unique<char[]>(size_ + 1))
    {
        memcpy(data_.get(), str, size);
        data_[size_] = '\0';
    }

    string::string(size_t capacity) :
        capacity_(capacity),
        size_(0),
        data_(make_unique<char[]>(capacity_))
    {
        memset(data_.get(), 0, capacity_);
    }

    string::string(const string &that) :
        string(that.data_.get())
    {
    }

    string::string(string &&that) :
        capacity_(that.capacity_),
        size_(that.size_),
        data_(move(that.data_))
    {
    }

    string::~string()
    {
    }
}
