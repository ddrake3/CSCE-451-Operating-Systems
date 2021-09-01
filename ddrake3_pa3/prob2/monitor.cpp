/***************************************************************************************
 * Author: Derek Drake
 * Date: 03/22/2021
 * CSCE-451
 *
 * This is the monitor for part2
 *
 ***************************************************************************************/
 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include <semaphore.h>
 #include <sys/ipc.h>
 #include <sys/sem.h>
 #include <unistd.h>
 
 typedef struct {
   // condition variable fields
   int sharedResource;
   sem_t counter;
 } cond;

 void wait (cond *cond) {
   // give up exclusive access to monitor and suspend appropriate thread
   // implement either Hoare or Mesa paradigm
   int tempInt = cond->sharedResource;
   cond->sharedResource = tempInt - 1;

   sem_t tempSem = cond->counter;
   if(cond->sharedResource < 0) {
     sem_wait(&(tempSem));
   }
}

  void signal (cond *cond) {
    // unblock suspended thread at head of queue
    // implement either Hoare or Mesa paradigm
    int tempInt = cond->sharedResource;
    cond->sharedResource = tempInt + 1;

    sem_t tempSem = cond->counter;
    if(cond->sharedResource <= 0) {
      sem_post(&(tempSem));
    }
  }
