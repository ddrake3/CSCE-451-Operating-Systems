/***************************************************************************************
 * Author: Derek Drake
 * Date: 03/22/2021
 * CSCE-451
 *
 * This is a program will:
 * Solve the producer/consumer problem using semaphores.
 *
 ***************************************************************************************/

 /***************************************************************************************
  * Adapted from:
  * Author: jaseemabid
  * Title of source code: producer-consumer.c
  * Type: github repo
  * Web: https://gist.github.com/jaseemabid/1922623
  *
  ***************************************************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include <semaphore.h>
 #include <sys/ipc.h>
 #include <sys/sem.h>
 #include <unistd.h>

 void *producer(void *id);
 void *consumer(void *id);

 pthread_t pid, cid;

 sem_t mutex; // = 1
 sem_t empty; // = N
 sem_t full; // = 0;

 char* buffer;
 char itemToInsert = 'X';
 char tempItem;

 int bufferLength, numProducers, numConsumers, numItemsToInsert;

 static int sharedResource = 0;

 int main (int argc, char *argv[]) {
   // get the following from the command line:
   //   -b for buffer length in bytes(argv[2])
   //   -p number of producer threads(argv[4])
   //   -c number of consumer threads(argv[6])
   //   -i number of items to insert(argv[8])
   bufferLength = atoi(argv[2]);
   numProducers = atoi(argv[4]);
   numConsumers = atoi(argv[6]);
   numItemsToInsert = atoi(argv[8]);

   // create buffer & initialise semaphores
   buffer = (char *) malloc(sizeof(char) * bufferLength);
   sem_init(&mutex, 0, 1);
   sem_init(&full, 0, 0);
   sem_init(&empty, 0, bufferLength);

   // create -p # of producer threads & # of consumer threads
   int *pidList = new int[numProducers];
   int *cidList = new int[numConsumers];

   for(int i = 0; i < numProducers; i++) {
     pidList[i] = i + 1;
   }
   for(int i = 0; i < numConsumers; i++) {
     cidList[i] = i + 1;
   }

   for(int i = 0; i < numProducers; i++) {
     if(pthread_create(&pid, NULL, producer, &pidList[i])) {
        fprintf(stderr, "Creation of producer thread %d failed!\n", pidList[i]);
        return -1;
     }
   }
   for(int i = 0; i < numConsumers; i++) {
     if(pthread_create(&cid, NULL, consumer, &cidList[i])) {
       fprintf(stderr, "Creation of consumer thread %d failed!\n", cidList[i]);
       return -1;
     }
   }

   // clean up all processes
   for(int i = 0; i < numProducers; i++) {
     pthread_join(pid, NULL);
   }
   for(int i = 0; i < numConsumers; i++) {
     pthread_join(cid, NULL);
   }

   return 0;
 }

 void *producer(void *id) {
   int *currentId = (int *) id;

   while(1) {
     sem_wait(&empty);
     sem_wait(&mutex);

     // insert a random character into the first available slot in the buffer
     if(numItemsToInsert > 0) {
       buffer[sharedResource] = itemToInsert;
       fflush(stdout);
       printf("p:<%u>, item: %c, at %d\n", *currentId, itemToInsert, sharedResource);
       sharedResource++;
       numItemsToInsert--;
    }
     else {
       sem_post(&mutex);
       sem_post(&full);

       return 0;
     }

     sem_post(&mutex);
     sem_post(&full);
   }
 }

 void *consumer(void *id) {
   int* currentCid = (int*) id;

   while(1) {
     sem_wait(&full);
     sem_wait(&mutex);

     // remove character from the last used slot in the buffer
     if(sharedResource > 0) {
       sharedResource--;
       tempItem = buffer[sharedResource];
       buffer[sharedResource] = '\0';
       fflush(stdout);
       printf("c:<%u>, item: %c, at %d\n", *currentCid, tempItem, sharedResource);
     }
     else {
       sem_post(&mutex);
       sem_post(&empty);

       exit(0);
     }

     sem_post(&mutex);
     sem_post(&empty);
   }
   pthread_exit(NULL);
 }
