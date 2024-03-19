#include "CurrentThread.h"

#include <sys/syscall.h>
#include <syscall.h>

pid_t __thread les::currentThread::t_cached_tid = 0;

void les::currentThread::cacheTid() {
    if (t_cached_tid == 0) {
        t_cached_tid = syscall(SYS_gettid);
    }
}
