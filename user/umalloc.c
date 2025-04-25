#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator

typedef long Align;

union header {
    struct {
        union header *ptr;
        uint size;
    } s;
    Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void free(void *ap) {
    Header *bp, *p;

    // 将用户指针转换为指向块头的指针
    // 用户使用的内存在Header结构后面，所以这里减1获取Header位置
    bp = (Header *)ap - 1;
    
    // 在空闲链表中寻找适合插入的位置
    // 目标：找到p和p->s.ptr，使得 p < bp < p->s.ptr
    for(p = freep ; !(bp > p && bp < p->s.ptr) ; p = p->s.ptr)
        // 特殊情况：处理环形链表边界
        // 当p位于链表末端(p >= p->s.ptr)，且bp要么大于p要么小于p->s.ptr
        // 即bp应该插入在链表"环"的交接处
        if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;
    
    // 尝试与后面的块合并（向高地址方向）
    // 如果bp块的结束地址正好等于下一个空闲块的起始地址
    if(bp + bp->s.size == p->s.ptr) {
        // 合并：增加大小并跳过下一个块
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else
        // 不能合并，只是简单地链接
        bp->s.ptr = p->s.ptr;
    
    // 尝试与前面的块合并（向低地址方向）
    // 如果p块的结束地址正好等于bp块的起始地址
    if(p + p->s.size == bp) {
        // 合并：增加大小并链接到bp指向的下一个块
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else 
        // 不能合并，只是简单地链接
        p->s.ptr = bp;
    
    // 更新freep指向当前位置，优化下次搜索
    freep = p;
}

// 当现有空闲内存不足时，向操作系统请求更多内存
static Header* morecore(uint nu) {
    char *p;
    Header *hp;

    // 为减少系统调用次数，最小分配4096个单位
    // 这是一种优化策略，避免频繁调用sbrk
    if(nu < 4096)
        nu = 4096;
    
    // 调用sbrk系统调用向操作系统请求更多内存
    // nu * sizeof(Header)是请求的字节数
    // sbrk返回新分配区域的起始地址
    p = sbrk(nu * sizeof(Header));
    
    // 如果sbrk返回-1，表示内存分配失败
    if(p == (char *)-1)
        return 0;  // 返回NULL表示分配失败
    
    // 将获得的内存转换为Header指针
    hp = (Header *)p;
    
    // 设置块大小
    hp->s.size = nu;
    
    // 通过调用free将这块新内存加入空闲链表
    // hp+1指向Header之后的内存区域（即用户可用部分）
    free((void *)(hp + 1));
    
    // 返回空闲链表指针，现在它指向新分配区域附近
    return freep;
}

void *malloc(uint nbytes) {
    Header *p, *prevp;
    uint nunits;

    // 计算所需内存块大小（以Header为单位）
    // 加上Header自身大小，向上取整，并加1确保至少有一个单位用于存储数据
    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    
    // 首次调用时初始化空闲链表
    if((prevp = freep) == 0) {
        // 创建一个空闲链表的"哨兵"节点
        // base指向自己形成环形链表
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    
    // 遍历空闲链表寻找足够大的块
    for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        // 找到足够大的块
        if(p->s.size >= nunits) {
            // 情况1：块大小正好等于请求大小
            if(p->s.size == nunits)
                // 直接从链表移除此块
                prevp->s.ptr = p->s.ptr;
            else {
                // 情况2：块大于请求大小，需要分割
                // 先减少原块的大小
                p->s.size -= nunits;
                // 向后移动指针，指向分割出的新块
                p += p->s.size;
                // 设置新块的大小
                p->s.size = nunits;
            }
            // 更新freep为优化下次搜索
            freep = prevp;
            // 返回用户可用的内存区域（跳过Header）
            return (void *)(p + 1);
        }
        
        // 如果遍历了整个链表都没找到合适的块
        if(p == freep)
            // 尝试向系统请求更多内存
            if((p = morecore(nunits)) == 0)
                // 如果系统无法提供更多内存，返回NULL
                return 0;
    }
}