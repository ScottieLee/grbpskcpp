#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/time.h>

#include "timerLib.h"

static struct TIMER_EVENT *ptimer_head = NULL;

void *timer_task_thread(void *args)
{
 int ret;
 time_t t;
 struct TIMER_EVENT *ptimer;
 pthread_t taskID;
 struct timespec cond_time;
 long sec;
 long usec;

 ptimer = (struct TIMER_EVENT *)args;

 if (ptimer->timevalue > 1000000)
 {
  sec = (ptimer->timevalue/1000000);
  usec = (ptimer->timevalue%1000000);
 }
 else
 {
  sec = 0;
  usec = ptimer->timevalue;
 }

 taskID = pthread_self();
 pthread_detach(taskID);
 while(1)
 {
  time(&t);

  cond_time.tv_sec = t+sec;
  cond_time.tv_nsec = usec*1000;
  ret = pthread_cond_timedwait(&ptimer->cond,&ptimer->mutex, &cond_time);
  if (ret == 0)
  {
   if (ptimer->live == 0)
   {
    free(ptimer);
    pthread_exit((void *)NULL);
   }
  }
  else if (ret == ETIMEDOUT)
  {
   ptimer->func(ptimer->->tb, ptimer->pkt, ptimer->eof);
  }
  pthread_mutex_unlock(&ptimer->mutex);
 }
}

static void add_timer_tail(struct TIMER_EVENT *new_one)
{
 struct TIMER_EVENT *ptemp;

 assert(new_one != NULL);

 if (ptimer_head == NULL)
 {
  new_one->prev=new_one->next = NULL;
  ptimer_head = new_one;
  return;
 }

 for(ptemp=ptimer_head; ptemp->next!=NULL; ptemp=ptemp->next);

 ptemp->next = new_one;
 new_one->prev = ptemp;
 new_one->next = NULL;
}

void del_timer(TIMERID id)
{
 struct TIMER_EVENT *ptemp;

 for(ptemp=ptimer_head; ptemp!=NULL; ptemp=ptemp->next)
 {

      pthread_cleanup_push( (void(*)(void *))pthread_mutex_unlock, (void *)&(ptemp->mutex));
  pthread_mutex_lock(&ptemp->mutex);
  if (ptemp->id == id)
  {
   /* Delete header */
   if (ptemp == ptimer_head)
   {
    if (ptemp->next != NULL)
    {
     ptemp->next->prev = NULL;
     ptemp->live = 0;
     ptimer_head = ptemp->next;
     pthread_cond_signal(&ptemp->cond);
     return;
    }
    else
    {
     ptemp->live = 0;
     ptimer_head = NULL;
     pthread_cond_signal(&ptemp->cond);
     return;
    }
   }
   else if (ptemp->next == NULL)  /* delete tail */
   {
    ptemp->live = 0;
    ptemp->prev->next = NULL;
    pthread_cond_signal(&ptemp->cond);
    return;
   }
   else /* normal */
   {
    ptemp->live = 0;
    ptemp->prev = ptemp->next;
    pthread_cond_signal(&ptemp->cond);
    return;
   }
  }
  pthread_cleanup_pop( 1 );
 }
}

TIMERID add_timer(top_block_sptr tb, std::string& pkt, bool eof, functiontype func)
{
 struct TIMER_EVENT *ptimer;
 pthread_t taskID;
 int ret;
 struct TIMER_EVENT *ptemp;
 int dirty;

 ptimer = (struct TIMER_EVENT *)malloc(sizeof(struct TIMER_EVENT));
 if (ptimer == NULL)
 {
  return (-1);
 }

 pthread_mutex_init(&ptimer->mutex, NULL);
 pthread_cond_init(&ptimer->cond, NULL);
 ptimer->live = 1;
 ptimer->func = func;
 ptimer->timevalue = timevalue;
 do {
  ptimer->id = rand();
  dirty = 0;
  for(ptemp=ptimer_head; ptemp != NULL; ptemp=ptemp->next)
  {
   if( ptemp->id == ptimer->id)
   {
    dirty = 1;
    break;
   }
  }

 } while(dirty == 1);


 ret = pthread_create(&taskID, 0, timer_task_thread, (void *)ptimer);
 if (ret < 0)
 {
  perror("pthread_create");
  free(ptimer);
  return (-1);
 }
 add_timer_tail(ptimer);

 return ptimer->id;
}
