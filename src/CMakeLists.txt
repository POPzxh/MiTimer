project(TimerQueue)


add_library(TimerQueue STATIC 
                Reactor.cpp
                Epoller.cpp
                CurrentThread.cpp
                Channel.cpp
                TimerQueue.cpp
                TimeWheel.cpp
                Timer.cpp
            )

add_library(les::TimerQueue ALIAS TimerQueue)

target_include_directories( TimerQueue
    PUBLIC ${CMAKE_SOURCE_DIR}/src/
)
