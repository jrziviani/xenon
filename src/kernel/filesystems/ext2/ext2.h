#ifndef EXT2_H
#define EXT2_H

class ext2
{
private:
    void create_block();

public:
    void probe();
    bool mount();
};

#endif // EXT2_H