#ifndef __VM_H
#define __VM_H

#include "types.h"
#include "memory.h"

using namespace clib::type;
using namespace clib::memory;

// Website: https://github.com/bajdcc/MiniOS

/* virtual memory management */
// 虚存分配（二级页表分配方式）

// 参考：http://wiki.osdev.org/Paging

// 对于一个32位虚拟地址（virtual address）
// 32-22: 页目录号 | 21-12: 页表号 | 11-0: 页内偏移
// http://www.360doc.com/content/11/0804/10/7204565_137844381.shtml

/* 4k per page */
#define PAGE_SIZE 4096 

/* 页掩码，取高20位 */
#define PAGE_MASK 0xfffff000

/* 地址对齐 */
#define PAGE_ALIGN_DOWN(x) ((x) & PAGE_MASK)
#define PAGE_ALIGN_UP(x) (((x) + PAGE_SIZE - 1) & PAGE_MASK)

/* 分析地址 */
#define PDE_INDEX(x) (((x) >> 22) & 0x3ff)  // 获得地址对应的页目录号
#define PTE_INDEX(x) (((x) >> 12) & 0x3ff)  // 获得页表号
#define OFFSET_INDEX(x) ((x) & 0xfff)       // 获得页内偏移

// 页目录项、页表项用uint32表示即可
typedef uint32_t pde_t;
typedef uint32_t pte_t;

/* 页目录大小 1024 */
#define PDE_SIZE (PAGE_SIZE/sizeof(pte_t))
/* 页表大小 1024 */
#define PTE_SIZE (PAGE_SIZE/sizeof(pde_t))
/* 页表总数 1024*PTE_SIZE*PAGE_SIZE = 4G */
#define PTE_COUNT 1024

/* CPU */
#define CR0_PG  0x80000000 

/* pde&pdt attribute */
#define PTE_P   0x1     // 有效位 Present
#define PTE_R   0x2     // 读写位 Read/Write, can be read&write when set
#define PTE_U   0x4     // 用户位 User / Kern
#define PTE_K   0x0     // 内核位 User / Kern
#define PTE_W   0x8     // 写回 Write through
#define PTE_D   0x10    // 不缓存 Cache disable
#define PTE_A   0x20    // 可访问 Accessed
#define PTE_S   0x40    // Page size, 0 for 4kb pre page
#define PTE_G   0x80    // Ignored

/* 用户代码段基址 */
#define USER_BASE 0xc0000000
/* 用户数据段基址 */
#define DATA_BASE 0xd0000000
/* 用户栈基址 */
#define STACK_BASE 0xe0000000

struct __page__
{
    byte bits[4096];
};

class CVirtualMachine
{
public:
    CVirtualMachine(std::vector<LEX_T(int)>, std::vector<LEX_T(char)>);
    ~CVirtualMachine();

    void exec(int entry);

private:
    // 申请页框
    uint32_t pmm_alloc();
    // 初始化页表
    void vmm_init();
    // 虚页映射
    void vmm_map(uint32_t va, uint32_t pa, uint32_t flags);
    // 解除映射
    void vmm_unmap(pde_t *pgdir, uint32_t va);
    // 查询分页情况
    int vmm_ismap(uint32_t va, uint32_t *pa) const;

    template<class T = int> T vmm_get(uint32_t va);
    char* vmm_getstr(uint32_t va);
    template<class T = int> T vmm_set(uint32_t va, T);

private:
    /* 内核页表 = PTE_SIZE*PAGE_SIZE */
    pde_t *pgd_kern;
    /* 内核页表内容 = PTE_COUNT*PTE_SIZE*PAGE_SIZE */
    pde_t *pte_kern;
    /* 申请16MB物理内存(1 block=16B) */
    memory_pool<1024 * 1024> memory;
    /* 页表 */
    pde_t *pgdir{ nullptr };
};

#endif
