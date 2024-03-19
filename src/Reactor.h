#ifndef LES_REACTOR_H
#define LES_REACTOR_H

#include <atomic>
#include <functional>
#include <map>
#include <mutex>
#include <vector>

#include "Channel.h"
#include "Epoller.h"
#include "Noncopyable.h"
#include "TimerQueue.h"

namespace les {

class Reactor : Noncopyable {
public:
    using Functor = std::function<void()>;

    using ChannelSp  = Channel::ChannelSp;
    using ChannelWp  = Channel::ChannelWp;
    using ChannelMap = std::map<int, ChannelSp>;

    static constexpr int TIMEOUT = 10 * 1000;

    Reactor();

    ~Reactor();

    // 根据fd创建新的连接或者获取目前的连接
    ChannelWp getChannel(int fd);

    // 根据fd移除channel
    void removeChannel(int fd);

    bool update(const Channel &);

    // 在Reator中执行func 如果是在Reator线程中直接运行 否则在pending中等待
    void runInLoop(Functor func);

    void loop();

    bool isStopped() { return quit_; }

    void quit();

private:
    friend class Channel;

    pid_t                pid_;
    Epoller              poller_;          // IO多路复用器
    std::atomic_bool     quit_;            // 终止标记
    std::vector<Functor> pendingFunctors_; // 非epoll事件
    std::mutex           functorMtx_;      // 操作pendingFunctors的互斥量
    std::atomic_bool     flag_ = false;

    ChannelMap channels_;      // Channels connected to reactor
    int        wakeupfd_;      // To wake up loop
    ChannelWp  wakeupChannel_; // Channel of waking up;
    // TimerQueue timerQueue_;    // TimerQueue for timers

    void wakeup();

    void handleWakeUp(); // wake up callback

    Epoller &epoller() { return poller_; }; // Return epoller

    void dopendingFunctors();
};

} // namespace les

#endif // LES_REACTOR_H