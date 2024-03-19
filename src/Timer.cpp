#include "Timer.h"

#include <cstdint>
#include <cstring>
#include <sys/timerfd.h>
#include <unistd.h>
#include <utility>

#include "Reactor.h"

using les::Timer;

Timer::Timer(Reactor &reactor, int timeout, TimerCallBack timerCallback)
    : reactor_(reactor)
    , timerfd_(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC))
    , timerChannel_(reactor.getChannel(timerfd_))
    , timeout_(timeout)
    , timerCallBack_(std::move(timerCallback)) {
    setTimerfd();
    auto channel = timerChannel_.lock();
    if (channel) {
        channel->setReadCallback([&]() {
            uint64_t buf;
            read(timerfd_, &buf, sizeof buf);
            timerCallBack_();
        });
        channel->enableReading();
    }
}

void Timer::setTimeout(int timeout) {
    timeout_ = timeout;
    setTimerfd();
}

void Timer::disable() {
    auto channel = timerChannel_.lock();
    if (channel) {
        channel->disableReading();
    }
}

void Timer::setTimerfd() {
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memset(&newValue, 0, sizeof newValue);
    memset(&oldValue, 0, sizeof oldValue);
    newValue.it_value.tv_sec = timeout_;
    // newValue.it_interval.tv_sec = timeout_;
    timerfd_settime(timerfd_, 0, &newValue, &oldValue);
}