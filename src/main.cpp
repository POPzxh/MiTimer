#include <chrono>
#include <iostream>
#include <mutex>
#include <sys/epoll.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "Channel.h"
#include "Reactor.h"
#include "TimeWheel.h"
#include "Timer.h"

static constexpr int TIME_OUT = 100;

void errHandler(OCI_Error *error) {
    printf("Code : ORA-%05i\n"
           "msg :%s\n"
           "sql :%s\n",
           OCI_ErrorGetOCICode(error), OCI_ErrorGetString(error),
           OCI_GetSql(OCI_ErrorGetStatement(error)));
}

int fetchInMonitor(les::TimeWheel *wheel, OCI_Connection *conn, OCI_Resultset *rs) {
    wheel->monitor(conn);
    int ret = OCI_FetchNext(rs);
    wheel->cancel(conn);
    return ret;
}

int fetchByTimer(OCI_Resultset *rs, les::Timer &timer) {
    timer.setTimeout(3);
    int ret = OCI_FetchNext(rs);
    return ret;
}

std::mutex mtx;

int fetch(OCI_Resultset *rs) {
    auto lock = std::lock_guard<std::mutex>(mtx);
    return OCI_FetchNext(rs);
}

void test() {
    OCI_Initialize(errHandler, nullptr, OCI_ENV_DEFAULT | OCI_ENV_THREADED);
    auto *cn = OCI_ConnectionCreate("172.28.8.10/fodb_ser", "TAC", "ca2804", OCI_SESSION_DEFAULT);
    les::Reactor   reactor;
    les::TimeWheel wheel(3, reactor);
    auto           loop = std::thread([&]() { reactor.loop(); });

    auto st = OCI_StatementCreate(cn);

    auto timer = les::Timer(reactor, TIME_OUT, [&]() { OCI_Break(cn); });
    OCI_Prepare(st, "SELECT LOCK_FLAG FROM tmi_plan_result WHERE UPDATE_TIME  > '20240307' AND "
                    "UPDATE_TIME < '20240308' ");
    // OCI_Prepare(st, "UPDATE TMI_PLAN_RESULT SET LOCK_FLAG = 1 WHERE IDENTIFIER = "
    //                 "'e93cf9cc67a34c839cbc387fb79e2e1f' ");
    OCI_Execute(st);
    auto *rs  = OCI_GetResultset(st);
    int   cnt = 0;
    auto  beg = std::chrono::system_clock::now();
    while (
        //
        // fetchInMonitor(&wheel, cn, rs)
        // OCI_FetchNext(rs)
        fetchByTimer(rs, timer) != 0
        //
    ) {
        ++cnt;
        OCI_GetInt2(rs, "LOCK_FLAG");
        // std::cout << cnt << std::endl;
    }
    auto end = std::chrono::system_clock::now();
    std::cout << cnt << ' '
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count() << "\n";
    timer.disable();
    reactor.quit();

    loop.join();
}

int main() {
    auto thread = std::thread();
    thread      = std::thread([]() { std::cout << "HELLO\n"; });
    thread.join();
}