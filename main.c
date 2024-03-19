#include "lib/MiTimer.h"
#include <stdio.h>
#include <unistd.h>

int cnt = 0;

void callback(void) {
    printf("TIME OUT %d\n", cnt);
}

int main(void) {
    MiReactor* reactor = MiReactorCreate();
    printf("%p\n", (void*)reactor);
    MiTimerCreate(reactor, 3, callback);
    // MiReactorStartLoop(reactor);
    MiTimerCreate(reactor , 1, callback);

    MiReactorStopLoop(reactor);
}
