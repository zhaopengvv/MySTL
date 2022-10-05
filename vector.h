#ifndef STL_VECTOR_H_
#define STL_VECTOR_H_


#include "stl_alloc.h"
#include "stl_construct.h"

namespace mystl {

template <class T, class Alloc = mystl::alloc>
class Vector {
public:
    // vector 的嵌套型别
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

protected:
    // 以下，simple_alloc是SGI STL的空间配置器
    typedef mystl::simple_alloc<value_type, Alloc> data_allocator;
    iterator start;             // 目前使用空间的头
    iterator finish;            // 目前使用空间的尾
    iterator end_of_storage;    // 目前可用空间的尾

    void insert_aux(iterator position, const T& x);
    void deallocate()
    {
        if (start) {
            data_allocator::deallocate(start, end_of_storage - start);
        }
    }

    void fill_initialize(size_type n, const T& value)
    {
        start = allocate_and_fill(n, value); // ???????????????
        finish = start + n;
        end_of_storage = finish;
    }

public:
    iterator begin()
    {
        return start;
    }

    iterator end()
    {
        return finish;
    }

    size_type size()
    {
        return size_type(end() - begin());
    }

    size_type capacity() const
    {
        return size_type(end_of_storage - begin());
    }

    bool empty() const
    {
        return begin() == end();
    }

    reference operator[](size_type n)
    {
        return *(begin() + n);
    }

    vector() : start(0), finish(0), end_of_storage(0) {}
    vector(size_type n, const T& value)
    {
        fill_initialize(n, value);
    }

    vector(int n, const T& value)
    {
        fill_initialize(n, value);
    }

    vector(long n, const T& value)
    {
        fill_initialize(n, value);
    }

    explicit vector(size_type n)
    {
        fill_initialize(n, T());
    }

    ~vector()
    {
        mystl::destroy(start, finish); // 析构
        deallocate(); // 释放内存空间
    }

    reference front()
    {
        return *begin();
    }

    reference back()
    {
        return *(end() -1);
    }

    void push_back(const T& x)
    {
        if (finish != end_of_storage) {
            mystl::construct(finish, x); // 全局函数
            ++finish;
        } else {
            insert_aux(end(), x);
        }
    }

    void pop_back()
    {
        --finish;
        mystl::destory(finish);
    }

    iterator erase(iterator position)
    {
        if (position + 1 != end()) {
            copy(poisition + 1, finish, position); // 后续元素往前移动 ???
        }
        --finish;
        mystl::destroy(finish);
        return position;
    }

  iterator erase(iterator __first, iterator __last) 
  {
        iterator __i = copy(__last, _M_finish, __first);
        destroy(__i, _M_finish);
        _M_finish = _M_finish - (__last - __first);
        return __first;
  }

    void resize(size_type new_size, const T& x)
    {
        if (new_size < size()) {
            erase(begin() + new_size, end());
        } else {
            insert(end(), new_size - size(), x);
        }
    }

    void resize(size_type new_size) 
    {
        resize(new_size, T()); // 未实现
    }

    void clear()
    {
        erase(begin(), end());
    }

protected:
    // 配置空间并填满内容
    iterator allocate_and_fill(size_type n, const T& x)
    {
        iterator result = data_allocator::allocate(n);
    }




};





}


#endif