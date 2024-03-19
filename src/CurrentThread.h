#ifndef LES_CURRENTTHREAD_H
#define LES_CURRENTTHREAD_H

#include <cstdint>
#include <unistd.h>

namespace les::currentThread {

extern __thread int t_cached_tid;

void cacheTid();

inline pid_t tid() {
    if (__builtin_expect(static_cast<int64_t>(t_cached_tid == 0), 0) != 0) {
        cacheTid();
    }
    return t_cached_tid;
}

} // namespace les::currentThread

#endif // LES_CURRENTTHREAD_H