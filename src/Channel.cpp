#include "Channel.h"

#include <cstring>
#include <sys/epoll.h>

#include "Reactor.h"

using les::Channel;

Channel::Channel(Reactor &reactor, int fd)
    : reactor_(reactor)
    , fd_(fd)
    , events_(0)
    , revents_(0) {}

void Channel::enableReading() {
    events_ |= READ_EVENT;
    update();
}

void Channel::disableReading() {
    events_ &= ~READ_EVENT;
    update();
}

void Channel::update() {
    if (status_ == DISABLE) {
        return;
    }
    struct epoll_event event;
    memset(&event, 0, sizeof event);
    event.events   = events_;
    event.data.ptr = this;

    int opteration = status_ == CLOSE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    reactor_.epoller().updateEvent(fd_, event, opteration);

    status_ = CONNECTED;
}

void Channel::handleEvent() {
    if ((revents_ & (POLLIN | POLLPRI | POLLRDHUP)) != 0) {
        if (readCallback_) {
            readCallback_();
        }
    }
    if ((revents_ & POLLOUT) != 0) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}

void Channel::disConnect() {
    if (status_ == CONNECTED) {
        struct epoll_event event;
        memset(&event, 0, sizeof event);
        reactor_.epoller().updateEvent(fd_, event, EPOLL_CTL_DEL);
    }
    status_ = DISABLE;
    reactor_.removeChannel(fd_);
}