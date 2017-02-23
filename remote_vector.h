// Copyright (c) 2017 Jeremy Rubin
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef _REMOTEVECTOR_H_
#define _REMOTEVECTOR_H_
#include <memory>
#include <cstddef>
#include <cstdlib>
template <typename T>
struct remote_vectorbase {
    T * __begin;
    T * __end;
    T * __real_end;
    remote_vectorbase<T>* resize(size_t n)
    {
        size_t size = __end-__begin;
        auto raw = std::realloc((void*) this, n*sizeof(T)+3*sizeof(T*));
        auto new_this = (remote_vectorbase*) raw;
        new_this->__begin = (T *) ((uintptr_t) (raw) + 3*sizeof(T*));
        new_this->__real_end = new_this->__begin + n;
        new_this->__end = new_this->__begin + (size > n ? n : size);
        while (new_this->__end != new_this->__real_end)
            new (new_this->__end++) T();
        return new_this;
    }
    T& operator[](size_t index)
    {
        return *(__begin+index);
    }

    const T& operator[](size_t index) const
    {
        return *(__begin+index);
    }

    remote_vectorbase<T>* reserve(size_t n) 
    {
        if (n > (__real_end - __begin)) {
            auto raw = std::realloc((void*) this, n*sizeof(T)+3*sizeof(T*));
            auto new_this = (remote_vectorbase*) raw;
            auto new_begin = (T *) ((uintptr_t) (raw) + 3*sizeof(T*));
            new_this->__real_end = new_begin + n;
            new_this->__end = new_begin + (new_this->__end - new_this->__begin);
            new_this->__begin = new_begin;
            return new_this;
        }
        return this;

    }

    size_t capacity() const
    {
        return __real_end - __begin;
    }

    size_t size() const
    {
        return __end - __begin;
    }

    template <typename ... Args>
    remote_vectorbase<T>* emplace_back(Args&&... args)
    {
        if (__end < __real_end)
            new (__end++) T(std::forward<Args>(args)...);
        else {
            auto s = __real_end - __begin;
            auto r = reserve(s<<1);
            return r->emplace_back(std::forward<Args>(args)...);
        }
        return this;
    }
    template <typename ... Args>
    remote_vectorbase<T>* push_back(Args... args)
    {
        if (__end < __real_end)
            *(__end++) = T(args...);
        else {
            auto s = __real_end - __begin;
            auto r = reserve((s<<1));
            return r->push_back(args...);
        }
        return this;
    }
    bool empty() const
    {
        return __begin == __end;
    }

    T* begin()
    {
        return __begin;
    }
    T* end()
    {
        return __end;
    }
    T& back() 
    {
        return *(__end-1);
    }
    T& front() 
    {
        return *__begin;
    }
    const T* begin() const
    {
        return __begin;
    }
    const T* end() const
    {
        return __end;
    }
    const T& back() const
    {
        return *(__end-1);
    }
    const T& front() const
    {
        return *__begin;
    }
    const T* cbegin() const
    {
        return __begin;
    }
    const T* cend() const
    {
        return __end;
    }
    void erase()
    {
        __end = __begin;
    }
    void pop_back()
    {
        --__end;
    }

};
template <typename T>
class remote_vector {
    remote_vectorbase<T> * ptr;
    public:
    remote_vector() : ptr((remote_vectorbase<T>*)std::malloc(3*sizeof(T *) + sizeof(T)))
    {
        ptr->__begin = (T*) (((uintptr_t) ptr) + 3*sizeof(T *));
        ptr->__end = ptr->__begin;
        ptr->__real_end = ptr->__begin+1;
    };
    ~remote_vector()
    {
        std::free(ptr);
    }
    void swap(remote_vector<T>& other)
    {
        std::swap(other.ptr, ptr);
    };
    void resize(size_t n)
    {
        ptr = ptr->resize(n);
    }
    T& operator[](size_t index)
    {
        return (*ptr)[index];
    }

    const T& operator[](size_t index) const
    {
        return (*ptr)[index];
    }

    void reserve(size_t n) 
    {
        ptr = ptr->reserve(n);
    }

    size_t capacity() const
    {
        return ptr->capacity();
    }

    size_t size() const
    {
        return ptr->size();
    }

    template <typename ... Args>
    void emplace_back(Args&&... args)
    {
       ptr = ptr->emplace_back(std::forward<Args>(args)...);
    }
    template <typename ... Args>
    void push_back(Args&&... args)
    {
        ptr = ptr->push_back( std::forward<Args>(args)...);
    }
    bool empty() const
    {
        return ptr->empty();
    }

    T* begin()
    {
        return ptr->begin();
    }

    T* end() 
    {
        return ptr->end();
    }
    T& back() 
    {
        return ptr->back();
    }
    T& front() 
    {
        return ptr->front();
    }
    const T* begin() const
    {
        return ptr->begin();
    }
    const T* end() const
    {
        return ptr->end();
    }
    const T& back() const
    {
        return ptr->back();
    }
    const T& front() const
    {
        return ptr->front();
    }
    const T* cbegin() const
    {
        return ptr->cbegin();
    }
    const T* cend() const
    {
        return ptr->cend();
    }
    void erase()
    {
        ptr->erase();
    }
    void pop_back()
    {
        ptr->pop_back();
    }
};


#endif
