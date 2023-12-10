#ifndef MY_STL_CONSTRUCT_H
#define MY_STL_ALLOCATOAR_H
#include <new>
#include <type_traits>

namespace mystl {

/**
 * 使用 placement new 在已经分配的特定空间（内存），调用对象的构造函数创建对象
 * ::new (void *p) T(value)
*/

template <class T>
void construct(T *ptr)
{
    ::new ((void *)ptr) T();
}

template <class T1, class T2>
void construct(T1* ptr, const T2& value)
{
    ::new ((void *)ptr) T1(value);
}

template <class T>
void destroy_one(T*, std::true_type) {}

template <class T>
void destroy_one(T *pointer, std::false_type)
{
    if (pointer != nullptr) {
        pointer->~T();
    }
}

/**
 * __type_traits：负责萃取型别（type）的特性。
 * 判断型别是否具备 non-trivial default ctor, non-trivial copy ctor, non-trivial assignment oreator, 
 * non-trivial dtor？如果不具备，在对型别进行析构、构造、拷贝和赋值等操作时，就可以采用最有效的措施，采用内存
 * 最直接的操作，如malloc和memcpy等，获得最高效率。 
*/

template <class T>
void destroy(T *pointer)
{
    destroy_one(pointer, std::is_trivially_destructible<T>{});
}

template <class ForwardIter>
void destroy(ForwardIter first, ForwardIter last)
{
    destroy_cat(first, last, std::is_trivially_destructible<typename iterator_traits<ForwardIter>::value_type>{});
}

template <class ForwardIter>
void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

template <class ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
{
    for (; first != last; ++first)
        destroy(&*first);
}


}; // namespace mystl
#endif