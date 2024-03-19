#ifndef LES_TIMERQUEUE_H
#define LES_TIMERQUEUE_H

#include "Channel.h"
#include "Noncopyable.h"

namespace les {

class Reactor;

class TimerQueue : Noncopyable {
public:
    using ChannelSp = Channel::ChannelSp;
    using ChannelWp = Channel::ChannelWp;

    TimerQueue(Reactor &reactor);

private:
    Reactor  &reactor_;
    int       timerfd_;
    ChannelWp timerChannel_;

    // handle timeout events
    void handleTimeOut();
};
} // namespace les

#endif // LES_TIMERQUEUE_H