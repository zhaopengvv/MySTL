#ifndef MYSTL_UNINITIALIZED_H_stl_construct
#define MYSTL_UNINITIALIZED_H_
#include <string.h>
#include "iterator.h"
#include "type_traits.h"
#include "construct.h"

namespace mystl {


/**
 * @brief 在内存块上构造元素
 * @param first 指向输入端的起始位置
 * @param last 指向输出端的起始位置（前闭后开区间）
 * @param result 指向输出端（欲初始化空间）的起始处
 * @return ** template<class InputIterator, class ForwardIterator> 
 */
template<class InputIterator, class ForwardIterator>
ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last,
                   ForwardIterator result)
{
    return __uninitialized_copy(first, last, result, value_type(result));
}

template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last,
                                            ForwardIterator result, T*)
{
    typedef typename __type_traits<T>::is_POD_type is_POD;
    // 使用is_POD所获得的结果，让编译器做参数推导
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
                         ForwardIterator result, __true_type)
{
    return copy(first, last, result);
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
                         ForwardIterator result, __false_type)
{
    ForwardIterator cur = result;
    // 省略异常处理
    for (; first != last; ++first, ++cur) {
        mystl::construct(&*cur, *first); // 元素必须一个一个地构造，无法批量进行
    }
    return cur;
}

// 以下是针对char * 和 wchar_t * 两种型别的特化版本
inline char* uninitialized_copy(const char* first, const char* last, char *result)
{
    memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t *result)
{
    memmove(result, first, sizeof(wchar_t) *(last - first));
    return result + (last - first);
}


/**
 * @brief 
 * @param first 指向输出端的起始处 
 * @param last 指向输出端的结束处
 * @param x 表示初值
 * @return void 
 */
template<class ForwardIterator, class T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last,
                        const T& x)
{
    __uninitialized_fill(first, last, value_type(first));
}

template <class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    __uninitialized_fill_aux(first, last, x, is_POD);
}

template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                                     const T& x, __true_type)
{
    fill(first, last, x);
}

template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                                     const T& x, __false_type)
{
    ForwardIterator cur = first;
    for (; cur != last; ++cur) {
        mystl::construct(&*cur, x); // 元素必须一个一个地构造，无法批量进行
    }
}

/**
 * @brief 初始化空间
 * @param first 指向欲初始化空间的起始处
 * @param n 表示欲初始化空间的大小
 * @param x 表示初值
 * @return ** template <class ForwardIterator, class Size, class T> 
 */
template <class ForwardIterator, class Size, class T>
ForwardIterator
unintialized_fill_n(ForwardIterator first, Size n, const T& x)
{
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD);
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
{
    return fill_n(first, n, x); // 交由高阶函数执行 stl_algobase.h
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
{
    // 省略异常处理
    ForwardIterator cur = first;
    for (; n > 0; --n, ++cur)
    {
        mystl::construct(&*cur, x);
    }
    return cur;
}








};



#endif 