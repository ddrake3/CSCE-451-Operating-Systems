/***************************************************************************************
 * Author: Derek Drake
 * Date: 03/22/2021
 * CSCE-451
 *
 * This is a program will:
 * Solve the producer/consumer problem using a monitor.
 *
 ***************************************************************************************/

  #include <stdio.h>
  #include <stdlib.h>
  #include <pthread.h>
  #include <semaphore.h>
  #include <sys/ipc.h>
  #include <sys/sem.h>
  #include <unistd.h>
  #include "monitor.cpp"

 void *producer(void *id);
 void *consumer(void *id);
 char generateRandomAlphabet();

 pthread_t pid, cid;

 sem_t mutex; // = 1
 cond empty; // = N
 cond full; // = 0;

 char* buffer;
 char tempItem;

 int bufferLength, numProducers, numConsumers, numItemsToInsert;

 static int shared = 0;

 int main (int argc, char *argv[]) {
   // get the following from the command line:
   //   -b for buffer length in bytes(argv[2])
   //   -p number of producer threads(argv[4])
   //   -c number of consumer threads(argv[6])
   //   -i number of items to insert(argv[8])
   bufferLength = atoi(argv[2]);
   int numProducers = atoi(argv[4]);
   int numConsumers = atoi(argv[6]);
   numItemsToInsert = atoi(argv[8]);

   // create buffer & initialise semaphore
   buffer = (char *) malloc(sizeof(char) * bufferLength);
   sem_init(&mutex, 0, 1);
   full.sharedResource = 0;
   empty.sharedResource = bufferLength;
   sem_init(&(full.counter), 0, 0);
   sem_init(&(empty.counter), 0, bufferLength);

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

   // join all threads
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
   char alpha;

   while(1) {
     wait(&empty);
     sem_wait(&mutex);

     // insert a random character into the first available slot in the buffer
     if(numItemsToInsert > 0) {
       alpha = generateRandomAlphabet();
       buffer[shared] = alpha;
       fflush(stdout);
       printf("p:<%u>, item: %c, at %d\n", *currentId, alpha, shared);
       shared++;
       numItemsToInsert--;
    }
     else {
       sem_post(&mutex);
       signal(&full);

       return 0;
     }

     sem_post(&mutex);
     signal(&full);
   }
 }

 void *consumer(void *id) {
   int* currentCid = (int*) id;

   while(1) {
     wait(&full);
     sem_wait(&mutex);

     // remove character from the last used slot in the buffer
     if(shared > 0) {
       shared--;
       tempItem = buffer[shared];
       buffer[shared] = '\0';
       fflush(stdout);
       printf("c:<%u>, item: %c, at %d\n", *currentCid, tempItem, shared);
     }
     else {
       sem_post(&mutex);
       signal(&empty);

       exit(0);
     }

     sem_post(&mutex);
     signal(&empty);

   }
 }

 char generateRandomAlphabet(){
   char randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random () % 26];
   return randomletter;
 }
