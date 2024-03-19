#ifndef LES_TIMER_H
#define LES_TIMER_H

#include <functional>

#include "Channel.h"
#include "Noncopyable.h"
#include "Reactor.h"
namespace les {

class Timer : Noncopyable {
public:
    using TimerCallBack = std::function<void()>;
    using ChannelWp     = Channel::ChannelWp;

    Timer(Reactor &reactor, int timeout, TimerCallBack callback);

    void setTimeout(int timeout);

    void disable();

private:
    Reactor  &reactor_;
    int       timerfd_;
    ChannelWp timerChannel_;

    int           timeout_;
    TimerCallBack timerCallBack_;

    void setTimerfd();
};

} // namespace les

#endif // LES_TIMER_H