#ifndef TIMERLIB_H
#define TIMERLIB_H
#include "top_block.h"
 typedef int TIMERID;

 typedef void (*functiontype) (top_block_sptr, std::string& , bool);

 struct TIMER_EVENT {
    struct TIMER_EVENT *prev, *next;
    TIMERID id;
    char name[64];
    int live;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    long timevalue;
    void (*func)(void *args);
 };
 /* Add a timer with name, timevalue(uSeconds) and handler */
 TIMERID add_timer(top_block_sptr, std::string& , bool, functiontype );
 /* Delete a timer with id */
 void del_timer(TIMERID id);

#endif // TIMERLIB_H
