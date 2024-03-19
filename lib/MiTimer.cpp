#include "MiTimer.h"

#include "Reactor.h"
#include "Timer.h"
#include <thread>

struct MiReactor {
    les::Reactor reactor_;
    std::thread  loopThread_;

    MiReactor()
        : loopThread_([&]() { reactor_.loop(); }) {}
};

struct MiTimer {
    MiTimer(MiReactor *reactor, int timeout, const les::Timer::TimerCallBack &callback)
        : timer_(reactor->reactor_, timeout, callback) {}

    les::Timer timer_;
};

void print() {}

MiReactor *MiReactorCreate() { return new struct MiReactor; }

void MiReactorStartLoop(MiReactor *reactor) {
    // if (reactor->reactor_.isStopped()) {
    reactor->loopThread_ = std::thread([&]() { reactor->reactor_.loop(); });
    // }
}

void MiReactorStopLoop(MiReactor *reactor) {
    reactor->reactor_.quit();
    MiReactorJoin(reactor);
}

MiTimer *MiTimerCreate(MiReactor *reactor, int timeout, TimerCallback timerCallback) {
    return new struct MiTimer(reactor, timeout, timerCallback);
}

void MiReactorJoin(MiReactor *reactor) {
    if (reactor->loopThread_.joinable()) {
        reactor->loopThread_.join();
    }
}