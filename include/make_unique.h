#pragma once
#include <memory>

// If using C++11, define make_unique (remove if compiling with C++14 or later)
#if __cplusplus <= 201103L
namespace std
{
    template <class T, class... Args>
    std::unique_ptr<T> make_unique(Args &&...args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
#endif