#include "TimerQueue.h"

#include <sys/timerfd.h>

#include "Reactor.h"

using namespace les;

TimerQueue::TimerQueue(Reactor &reactor)
    : reactor_(reactor)
    , timerfd_(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC))
    , timerChannel_(reactor_.getChannel(timerfd_)) {
    auto p = timerChannel_.lock();
    p->enableReading();
}