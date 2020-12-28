#ifndef EXT2_H
#define EXT2_H

namespace xenon
{
    class ext2
    {
    private:
        void create_block();

    public:
        void probe();
        bool mount();
    };
}

#endif // EXT2_H
