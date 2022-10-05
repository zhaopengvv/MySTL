#ifndef MYSTL_ALLOC_H_
#define MYSTL_ALLOC_H_

#include <stddef.h>

namespace mystl {

# ifdef __USE_MALLOC
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;
# else 
// 令alloc为第二级配置器
typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;

#endif

// allocate 统一接口
template<class T, class Alloc>
class simple_alloc {
public:
    static T *allocate(size_t n)
    {
        return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
    }

    static T *allocate(void)
    {
        return (T*)Alloc::allocate(sizeof(T));
    }

    static T *deallocate(T *p, size_t n)
    {
        if (0 != n) {
            Alloc::deallocate(p, n * sizeof(T));
        }
    }

    static T *deallocate(T *p)
    {
        Alloc::deallocate(p, sizeof(T));
    }
};

// 第一级配置器
template <int inst>
class __malloc_alloc_template {
private:
    // 以下函数处理内存不足的情况
    // oom : out of memory
    static void *my_oom_malloc(size_t)
    {
        void (* my_malloc_handler)();
        void *result;
        for (;;) {
            my_malloc_handler = __malloc_alloc_omm_handler;
            if (0 == my_malloc_handler) {
                __THROW_BAD_ALLOC;
            }
            (* my_malloc_handler)(); // 调用处理例程，企图释放内存
            result = malloc(n);
            if (result) {
                return result;
            }
        }
    }
    static void *my_oom_realloc(void*, size_t)
    {
        void (* my_malloc_handler)();
        void *result;
        for (;;) {
            my_malloc_handler = __malloc_alloc_omm_handler;
            if (0 == my_malloc_handler) {
                __THROW_BAD_ALLOC;
            }

            (* my_malloc_handler)(); // 调用处理例程，企图释放内存
            result = realloc(p, n);
            if (result) {
                return result;
            }
        }
    }
    static void (* my__malloc_alloc_oom_handler)() = 0;

public:
    static void *allocate(size_t n)
    {
        void *result = malloc(n); // 第一级配置器直接使用melloc
        // 一下无法满足要求时，改用omm_malloc()
        if (result == 0) {
            result = my_oom_malloc(n);
        }
        return result;
    }

    static void deallocate(void *p, size_t /* n */)
    {
        free(p); // 第一级配置器直接使用free()
    }

    static void *reallocate(void *p, size_t /*old_sz*/, size_t new_sz)
    {
        void *result = realloc(p, new_sz);
        // 一下无法满足要求时，改用omm_malloc()
        if (result == 0) {
            result = my_oom_realloc(p, new_sz);
        }
        return result;
    }

    // 仿真C++的set_new_handler() 
    // 可以通过它指定自己的out-of-memory handler
    static void (* set_malloc_hander(void (*f)())) ()
    {
        void (* old()) () = my__malloc_alloc_oom_handler;
        my__malloc_alloc_oom_handler = f;
        return old;
    }
};

typedef __malloc_alloc_template<0> malloc_alloc;

/*******************************************************************************************/
// 第二级配置器

enum {__ALIGN = 8}; // 小型区块的上调边界
enum {__MAX_BYTES = 128}; // 小型区块的上限
enum {__NFREELISTS = __MAX_BYTES/__ALIGN}; // free-lists 个数

template <bool threads, int inst>
class __default_alloc_template {
private:
    // ROUND_UP() 将bytes上调至8的倍数
    static size_t ROUND_UP(size_t bytes) {
        return (((bytes) + __ALIGN -1) & ~(__ALIGN -1));
    }

private:
    union obj {
        union obj *free_list_link; // 指向相同形式的另一个obj
        char client_data[i]; // 指向实际区块
    };

private:
    static obj *volatile free_list[__NFREELISTS];
    // 根据区块大小，决定使用第n号free-list。n从0开始
    static size_t FREELIST_INDEX(size_t bytes)
    {
        return (((bytes) + __ALIGN -1) / __ALIGN - 1);
    }
    
    // 返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
    static void *refill(size_t n);

    // 配置一个大空间，可容纳nobjs个大小为“size”的区块
    // 如果配置nobjs个区块有所不便，nobjs可能会降低
    static char *chunk_alloc(size_t size, int &nobjs);

    // Chunk allocation state
    static char *start_free; // 内存池起始位置
    static char *end_free;  // 内存池结束位置
    static size_t heap_size;

    static void *allocate(size_t n)
    {
        obj *volatile *my_free_list;
        obj *result = nullptr;
        // 大于128就调用第一级配置器
        if (n > (size_t)__MAX_BYTES) {
            return (malloc_alloc::allocate(n);)
        }
        // 寻找16个free list中适当的一个
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == 0) { // 无可用的区块
            void *r = refill(ROUND_UP(n));
            return r;
        }
        // 调整free list
        *my_free_list = result->free_list_link;
        return (result);
    }

    static void *deallocate(void *p, size_t n)
    {
        obj *q = (obj *)p;
        obj *volatile *my_free_list;
        // 大于128就调用第一级配置器
        if (n > (size_t)__MAX_BYTES) {
            malloc_alloc::deallocate(p, n);
            return;
        }
        // 寻找对应的free list
        my_free_list = free_list + FREELIST_INDEX(n);
        // 调整free list，回收区块
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }

};

// static data member 的定义与初值设定
template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;
char *__default_alloc_template<thread, inst>::end_free = 0;
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
__default_alloc_template<threads, inst>::obj * volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS]
    = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

template<bool threads, int inst>
void * __default_alloc_template<threads, inst>::refill(size_t n)
{
    int nobjs = 20;
    // 调用chunk_alloc()，尝试取得nobjs个区块作为free list的新节点
    // 注意参数nobjs是pass by reference
    char *chunk = chunk_alloc(n, nobjs);
    obj *volatile *my_free_list;
    obj *result;
    obj *current_obj, *next_obj;
    int i;

    // 如果只获取一个区块，这个区块分配给调用者用，free list无新节点
    if (1 == nobjs) {
        return chunk;
    }

    // 否则准备调整free list，纳入新节点
    my_free_list = free_list + FREELIST_INDEX(n);
    // 以下在chunk空间内建立free list
    result = (obj*)chunk; // 这1块准备返回给客端
    // 以下导引free list指向新配置的空间（取自内存池）
    *my_free_list = next_obj = (obj *)(chunk + n);
    // 以下将free list的各个节点串接起来
    for (i = 1; ; i++) { // 从1开始，因为第0个将返回给客端
        current_obj = next_obj;
        next_obj = (obj *)((char *)next_obj + n);
        if (nobjs - 1 == i) {
            current_obj->free_list_link = 0;
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }

    return (result);
}


/**
 * @brief 内存池
 * 以end_free - start_free来判断内存池的水量，如果水量充足，就直接调出20个区块，返回给freelist
 * 如果水量不足以提供20个区块，但是还足够供应一个以上的区块，就拨出这个不足20个区块的空间出去，
 * nobjs的参数值被修改为实际能够供应的区块数。
 * 如果水量不足以提供1个区块，对客端显然无法交代，此时需要利用malloc()从heap中配置内存，
 * 为内存吃注入活水源头，应对需求。新水量的大小为需求量的2倍，再加上上一个随着配置次数增阿基而愈来愈大的附加值。
 * @param size 区块的大小，已经上调为8的倍数
 * @param nobjs 区块的数量
 * @return char*
 */
template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &nobjs)
{
    char *result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free; // 内存池剩余空间

    if (bytes_left >= total_bytes) {
        // 内存池剩余空间完全满足需求量
        result = start_free;
        start_free += total_bytes;
        return result;
    } else if (bytes_left >= size) {
        // 内存池剩余空间不能完全满足需求量，但足够供应一个（含）以一个的区块
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;
    } else {
        // 内存池剩余空间连一个区块的大小都无法提供
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(head_size >> 4);
        // 以下让内存池中的残余零头还有利用价值
        if (bytes_left > 0) {
            // 内存池内还有一些零头，先配给适当的free list
            // 首先寻找适当的free list
            obj * volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
            // 调整free list ，将内存池中残存空间编入
            ((obj*)start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;
        }

        // 配置heap空间，用来补充内存池
        start_free = (char *)malloc(bytes_to_get);
        if (start_free == 0) {
            // heap空间不足，malloc失败
            int i;
            obj *volatile *my_free_list, *p;
            // 试着检视我们手上拥有的东西，这不会造成伤害。
            // 我们不打算尝试配置较小的区块，因为那在多线程机器上容易导致灾难
            // 以下搜寻适当的free list
            // 所谓适当是指“尚有未用区块，且区块够大”的free list
            for (i = size; i <= __MAX_BYTES; i+=__ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if (p != 0) {
                    // 调整 free list 以释放出未用区块
                    *my_free_list = p->free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    // 递归调用自己，为了修正nobjs
                    return (chunk_alloc(size, nobjs));
                    // 注意：任何残余零头终将被编入适当的free-list中备用
                }
            }
            end_free = 0; // 如果出现意外（到处都没有内存了）
            // 调用第一级配置器，看看oom机制是否能尽点力
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            // 这会导致抛出异常（exception）,或内存不足的情况获得改善
        }

        heap_size += bytes_to_get;

        end_free = start_free + bytes_to_get;
        return (chunk_alloc(size, nobjs));
    }

}


}



#endif