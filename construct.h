#ifndef MYSTL_STLCONSTRUCT_H_
#define MYSTL_STLCONSTRUCT_H_
#include <new>
#include "type_traits.h"

// 内存配置后的对象构造和内存释放前的对象析构
namespace mystl {

template <class T1, class T2>
inline void construct(T1* p, const T2& value)
{
    new (p) T1(value); // placement new; 调用T1:T1(value)，在指定空间生构造对象
}

// destroy 第一版本，接受一个指针
template <class T>
inline void destroy(T* pointer)
{
    pointer->~T(); // 调用dtor ~T();
}

// destroy 第二版本，接受两个迭代器。此函数设法找出元素的数值型别
// 进而利用__type_traits<>求取最适当的措施
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
    __destroy(first, last, value_type(first));
}

// 判断元素数值型别（value type）是否有 trivial destructor
template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
     // 先使用value_type获取迭代器所指对象的型别，
     // 再使用__type_traits<T>判断该型别的析构函数是否has_trival_destructor
    typedef typename mystl::__type_traits<T>::has_trival_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor()); // 编译时确定调用函数
}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
    for (; first < last; ++first) {
        destroy(&*first);
    }
}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type)
{}

// destroy 第二版本，针对迭代器为char* 和wchar_t*的特化版
inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}


}



#endif
