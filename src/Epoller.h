#ifndef LES_EPOLLER_H
#define LES_EPOLLER_H

#include <sys/epoll.h>
#include <vector>

#include "Channel.h"
#include "Noncopyable.h"

namespace les {

class Epoller : Noncopyable {

public:
    using EventList = std::vector<struct epoll_event>;

    static constexpr int EVENT_LIST_INIT_SIZE = 16;

    explicit Epoller(int timeout);

    ~Epoller();

    // update event
    void updateEvent(int fd, struct epoll_event &event, int operation);

private:
    friend class Reactor;

    int       epollfd_;
    EventList eventList_;
    int       timeout_;

    std::vector<Channel *> poll();
};

} // namespace les

#endif // LES_EPOLLER_H