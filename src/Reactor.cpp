#include "Reactor.h"

#include <cstdint>
#include <fcntl.h>
#include <functional>
#include <memory>
#include <mutex>
#include <sys/eventfd.h>
#include <unistd.h>
#include <vector>

#include "Channel.h"
#include "CurrentThread.h"
#include "Epoller.h"
#include "TimerQueue.h"

using les::Channel;
using les::Reactor;

// #define LOCK_FREE

Reactor::Reactor()
    : pid_(0)
    , poller_(TIMEOUT)
    , quit_(false)
    , wakeupfd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
    , wakeupChannel_(getChannel(wakeupfd_)) {
    if (wakeupfd_ < 0) {
        abort();
    }
    auto sp = wakeupChannel_.lock();
    sp->setReadCallback([this] { handleWakeUp(); });
    sp->enableReading(); // 注册唤醒事件
}

Reactor::~Reactor() { ::close(wakeupfd_); }

void Reactor::loop() {
    pid_ = currentThread::tid();
    while (!quit_) {
        auto channels = poller_.poll();
        for (auto channel : channels) {
            channel->handleEvent();
        }
        // dopendingFunctors();
    }
}

void Reactor::runInLoop(Functor func) {
    if (pid_ == currentThread::tid()) {
        func();
    } else {
#ifdef LOCK_FREE
        bool isDoign = false;
        while (!flag_.compare_exchange_weak(isDoign, true)) {
            isDoign = false;
        }
#else
        std::lock_guard<std::mutex> lock(functorMtx_);
#endif
        pendingFunctors_.push_back(std::move(func));
        wakeup();
#ifdef LOCK_FREE
        flag_ = false;
#endif
    }
}

Reactor::ChannelWp Reactor::getChannel(int fd) {
    auto valid = Channel::isValidFd(fd);
    auto it    = channels_.find(fd);
    if (it == channels_.end()) {
        if (!valid) {
            return {};
        }
        auto ret = channels_.emplace(fd, std::make_shared<Channel>(*this, fd));
        if (!ret.second) {
            return {};
        }
        it = ret.first;
    }
    if (!valid) {
        channels_.erase(it);
        return {};
    }
    return it->second;
}

void Reactor::removeChannel(int fd) { channels_.erase(fd); }

bool Reactor::update(const Channel &channel) {
    if (this == &channel.getReactor()) {
    }
    return false;
}

void Reactor::quit() {
    quit_ = true;
    if (currentThread::tid() != pid_) {
        wakeup();
    }
}

void Reactor::wakeup() {
    // write one byte into fd
    uint64_t one = 1;
    ::write(wakeupfd_, &one, sizeof one);
}

void Reactor::handleWakeUp() {
    uint64_t one = 1;
    ::read(wakeupfd_, &one, sizeof one);
}

void Reactor::dopendingFunctors() {
    auto functors = std::vector<Functor>();
    {
#ifdef LOCK_FREE
        bool isDoign = false;
        while (!flag_.compare_exchange_weak(isDoign, true)) {
            isDoign = false;
        }
#else
        auto lock = std::lock_guard<std::mutex>(functorMtx_);
#endif
        functors.swap(pendingFunctors_);
#ifdef LOCK_FREE
        flag_ = false;
#endif
    }
    for (const auto &func : functors) {
        func();
    }
}