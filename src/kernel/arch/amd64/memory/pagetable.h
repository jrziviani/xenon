#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <klib/stdint.h>

void map_kernel_memory();

union pagetable_t
{
    struct fields {
        uint64_t present : 1;
        uint64_t rw : 1;
        uint64_t user : 1;
        uint64_t pwt : 1;
        uint64_t pcd : 1;
        uint64_t accessed : 1;
        uint64_t dirty : 1;
        uint64_t pat : 1;
        uint64_t granularity : 1;
        uint64_t available_1 : 3;
        uint64_t address : 40;
        uint64_t available_2 : 11;
        uint64_t noexec : 1;
    } fields;

    uint64_t ulong;
    uint8_t byte[8];
};

union dirtable_t {
    struct fields {
        uint64_t present : 1;
        uint64_t rw : 1;
        uint64_t user : 1;
        uint64_t pwt : 1;
        uint64_t pcd : 1;
        uint64_t accessed : 1;
        uint64_t ignored : 1;
        uint64_t zero : 2;
        uint64_t available_1 : 3;
        uint64_t address : 40;
        uint64_t available_2 : 11;
        uint64_t noexec : 1;
    } fields;

    uint64_t ulong;
    uint8_t byte[8];
};

struct pte_t {
    uintptr_t pages[512];
};

struct pde_t {
    uintptr_t dirs[512];
};

struct pdpt_t {
    uintptr_t dirs[512];
};

struct pml4_t {
    uintptr_t dirs[512];
};

#endif // PAGETABLE_H
