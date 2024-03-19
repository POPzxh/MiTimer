#ifndef LES_TIMEWHEEL_H
#define LES_TIMEWHEEL_H

#include "Channel.h"
#include "Noncopyable.h"

#include "ocilib.h"
#include <unordered_map>
#include <unordered_set>

namespace les {

class Reactor;

class TimeWheel : Noncopyable {
public:
    using ChannelWp = Channel::ChannelWp;
    using Bucket    = std::unordered_set<OCI_Connection *>;

    TimeWheel(int timeout, Reactor &reactor);

    void monitor(OCI_Connection *conn);

    void cancel(OCI_Connection *conn);

private:
    int                                       timeout_;
    Reactor                                  &reactor_;
    int                                       timerfd_;
    ChannelWp                                 timerChannel_;
    int                                       cur_;
    std::vector<Bucket>                       list_;
    std::unordered_map<OCI_Connection *, int> map_;

    int tail() { return cur_ == 0 ? timeout_ : cur_ - 1; }

    int next() { return cur_ == timeout_ ? 0 : cur_ + 1; }

    void setTimerfd();

    void readTimerfd();

    // tick per secound
    void tick();

    void monitorInLoop(OCI_Connection *conn);

    void cancelInLoop(OCI_Connection *conn);
};
} // namespace les

#endif // LES_TIMEWHEEL_H