#ifndef VFS_H
#define VFS_H

#include <stdint.h>

template <class FS>
class filesystem : public FS
{
    public:
        void open();
        void close();
        void read();
        void write();
};

#endif // VFS_H
