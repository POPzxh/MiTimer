#include "TimeWheel.h"

#include <bits/types/struct_itimerspec.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <sys/timerfd.h>
#include <unistd.h>

#include "Reactor.h"

using les::Reactor;
using les::TimeWheel;

TimeWheel::TimeWheel(int timeout, Reactor &reactor)
    : timeout_(timeout)
    , reactor_(reactor)
    , timerfd_(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC))
    , timerChannel_(reactor.getChannel(timerfd_))
    , cur_(0)
    , list_(timeout_ + 1) {
    setTimerfd();
    auto channel = timerChannel_.lock();
    channel->setReadCallback([this] { tick(); });
    channel->enableReading();
}

void TimeWheel::setTimerfd() {
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memset(&newValue, 0, sizeof newValue);
    memset(&oldValue, 0, sizeof oldValue);
    newValue.it_value.tv_sec    = timeout_;
    newValue.it_interval.tv_sec = timeout_;
    timerfd_settime(timerfd_, 0, &newValue, &oldValue);
}

void TimeWheel::readTimerfd() {
    uint64_t buf;
    ::read(timerfd_, &buf, sizeof buf);
}

void TimeWheel::monitor(OCI_Connection *conn) {
    reactor_.runInLoop([this, conn] { monitorInLoop(conn); });
}

void TimeWheel::cancel(OCI_Connection *conn) {
    reactor_.runInLoop([this, conn] { cancelInLoop(conn); });
}

void TimeWheel::cancelInLoop(OCI_Connection *conn) {
    // std::cout << "CANCEL[" << conn << "]\n";
    int pos = map_[conn];
    list_[pos].erase(conn);
    map_.erase(conn);
}

void TimeWheel::monitorInLoop(OCI_Connection *conn) {
    // std::cout << "MONITOR[" << conn << "]\n";
    if (map_.find(conn) == map_.end()) {
        int pos    = tail();
        map_[conn] = pos;
        list_[pos].insert(conn);
    }
}

void TimeWheel::tick() {
    // std::cout << "TICK[" << cur_ << "]\n";
    readTimerfd();
    cur_ = next();
    for (auto conn : list_[cur_]) {
        OCI_Break(conn);
        map_.erase(conn);
        std::cout << "BREAK CONNECTION [" << conn << "]";
    }
    list_[cur_].clear();
}