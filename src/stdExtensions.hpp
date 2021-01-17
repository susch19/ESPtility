#ifndef __STD_EXTENSIONS__
#define __STD_EXTENSIONS__
#if __cplusplus<201402

#include <memory>

namespace std
{
    template <typename T, typename... Args>
    static std::unique_ptr<T> make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
} // namespace std

#endif
#endif