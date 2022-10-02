#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_
#include <new>
#include <iostream>

namespace mystl {

template <class T>
inline T* allocate(ptrdiff_t size, T*)
{
    set_new_hander(0);
    // 分配内存，不构造对象
    T* tmp = (T*) (::operator new((size_t)(size * sizeof(T))));
    if (tmp == 0) {
        cerr << "out of memory" << endl;
        exit(0);
    }
    return tmp;
}

template <class T>
inline void deallocate(T* buffer)
{
    ::operator delete(buffer);
}


}

#endif