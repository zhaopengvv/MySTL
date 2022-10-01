// 通过如下demo了解Traits编程技法

# pragma once
#include <iostream>
using namespace std;

// 5种迭代器类型
struct input_itrerator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag {};
struct bidirectional_itreator_tag {};
struct random_access_iterator_tag {};

// 自定义Iterator
template<class T>
struct MyIter {
    // 内嵌型别声明
    typedef Category    iteratory_category;
    typedef T           value_type;
    typedef Distance    difference_type;
    typedef Pointer     pointer;
    typedef Reference   reference;


    T* ptr;
    MyIter(T* p = 0) : ptr(p) {}

    T& operator*() const 
    {
        return *ptr;
    }
    T* operator->() const 
    {
        return ptr;
    }

    T& operator++()
    {
        ptr = ptr->next();
        return *this;
    }

    T operator++(int)
    {
        T tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const T& i) const
    {
        return ptr == i.ptr;
    }

    bool operator!=(const T& i) const
    {
        return ptr != i.ptr;
    }

};

template<class I>
typename iterator_traits<I>::value_type
func(I iter)
{
    return *iter;
}

// 不论面对的是自定义类的迭代器，或是原生指针int *或是const int*都可以通过traits取出正确的value type
// 如果I定义有自己的value type，通过这特traits的作用，萃取出来的vaule_type，就是I::value_type
template<class I>
struct iterator_traits {
    typedef typename I::iteratory_category  iterator_category;
    typedef typename I::value_type          value_type;
    typedef typename I::difference_type     difference_type;
    typedef typename I::pointer             pointer;
    typedef typename I::reference           reference;
};

// 针对原生指针（native pointer）而设计的traits的偏特化版本
template<class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag  inerator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};

// 针对原生指针（pointer-to-const）而设计的traits的偏特化版本
template<class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag  inerator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};




