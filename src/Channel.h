#ifndef LES_CHANNEL_H
#define LES_CHANNEL_H

// 用于沟通Reactor和fd

#include <fcntl.h>
#include <functional>
#include <memory>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "Noncopyable.h"

namespace les {
class Reactor;

class Channel : Noncopyable {
public:
    using EventCallback = std::function<void()>;

    using ChannelSp = std::shared_ptr<Channel>;
    using ChannelWp = std::weak_ptr<Channel>;

    static constexpr int READ_EVENT  = EPOLLIN | EPOLLPRI;
    static constexpr int WRITE_EVENT = EPOLLOUT;

    static constexpr int CLOSE     = 0;
    static constexpr int CONNECTED = 1;
    static constexpr int DISABLE   = -1;

    Channel(Reactor &reactor, int fd);

    ~Channel() {
        if (isValid()) {
            ::close(fd_);
        }
    }

    // 从reactor中断开 删除Channel
    void disConnect();

    static bool isValidFd(int fd) { return fd >= 0 && fcntl(fd, F_GETFD) >= 0; };

    bool isValid() { return isValidFd(fd_); }

    const Reactor &getReactor() const { return reactor_; }

    void setEvent(int events) { events_ = events; }

    void setRevent(int revents) { revents_ = revents; }

    // set event type
    // 开始监听读
    void enableReading();
    // 读失效
    void disableReading();

    void setReadCallback(const EventCallback &cb) { readCallback_ = cb; };

    void setReadCallback(EventCallback &&cb) { readCallback_ = std::move(cb); };

    void setWriteCallback(const EventCallback &cb) { writeCallback_ = cb; };

    void setWriteCallback(EventCallback &&cb) { writeCallback_ = std::move(cb); };

    // 处理事件
    void handleEvent();

private:
    Reactor  &reactor_; // Reactor the fd will be registered
    const int fd_;      // fd managed by channel

    int status_; // current status of Channel

    int events_;  // events registerted to epoll
    int revents_; // received events from epoll

    EventCallback readCallback_;  // read callback function used whem read event triggered
    EventCallback writeCallback_; // write callback function used whem write
                                  // event triggered
    EventCallback closeCallback_; // close callback function used whem close
                                  // event triggered
    EventCallback errorCallback_; // error callback function used whem error
                                  // event triggered

    // 监听事件更新 向reactor中的epoll更新
    void update();
};
} // namespace les

#endif // LES_CHANNEL_H