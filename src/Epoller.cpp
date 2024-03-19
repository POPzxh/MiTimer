#include "Channel.h"

#include <cstddef>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#include "Epoller.h"

using les::Channel;
using les::Epoller;

Epoller::Epoller(int timeout)
    : epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , eventList_(EVENT_LIST_INIT_SIZE)
    , timeout_(timeout) {
    std::cout << "Create Poller\n";
}

Epoller::~Epoller() { ::close(epollfd_); }

std::vector<Channel *> Epoller::poll() {
    int eventNum =
        ::epoll_wait(epollfd_, eventList_.data(), static_cast<int>(eventList_.size()), timeout_);
    std::cout << "Wake up[" << eventNum << "]\n";
    auto channels = std::vector<Channel *>();
    if (eventNum > 0) {
        channels.reserve(eventNum);
        for (int i = 0; i < eventNum; ++i) {
            auto p = static_cast<Channel *>(eventList_[i].data.ptr);
            p->setRevent(eventList_[i].events);
            channels.push_back(p);
        }
        if (static_cast<size_t>(eventNum) == eventList_.size()) {
            // resize
            eventList_.resize(2 * eventNum);
        }
    } else if (eventNum == 0) {
        // nothing happened
        std::cout << "Nothing happened\n";
    } else {
        // error
    }
    return channels;
}

void Epoller::updateEvent(int fd, struct epoll_event &event, int operation) {
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        // Fail
        std::cout << operation << ' ' << epollfd_ << "FAILED\n";
    }
}