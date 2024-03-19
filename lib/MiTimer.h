#ifndef LES_MITIMER_H
#define LES_MITIMER_H

#ifdef __cplusplus
extern "C" {
#endif
// NOLINTBEGIN
typedef struct MiReactor MiReactor;

typedef struct MiTimer MiTimer;

typedef void (*TimerCallback)(void);

extern MiReactor *MiReactorCreate(void);

extern void MiReactorStartLoop(MiReactor *reactor);

extern void MiReactorStopLoop(MiReactor *reactor);

extern void MiReactorJoin(MiReactor *reactor);

extern MiTimer *MiTimerCreate(MiReactor *reactor, int timeout, TimerCallback timerCallback);

// extern void

// NOLINTEND
#ifdef __cplusplus
};
#endif

#endif // LES_MITIMER_H
