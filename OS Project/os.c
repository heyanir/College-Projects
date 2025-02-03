#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

int sharedData = 0;
int readersCount = 0;
sem_t mutex, writeBlock;
int exitFlag = 0;  

void* reader(void* arg) {
    while (!exitFlag) {
        sem_wait(&mutex);
        readersCount++;
        if (readersCount == 1) {
            sem_wait(&writeBlock);
        }
        sem_post(&mutex);

        printf("Reader %ld read: %d\n", (long)arg, sharedData);
        sem_wait(&mutex);
        readersCount--;
        if (readersCount == 0) {
            sem_post(&writeBlock);
        }
        sem_post(&mutex);

        usleep(rand() % 100000);
    }
    return NULL;
}
void* writer(void* arg) {
    while (!exitFlag) {
        sem_wait(&writeBlock);
        sharedData++;
        printf("Writer %ld wrote: %d\n", (long)arg, sharedData);
        sem_post(&writeBlock);

        usleep(rand() % 100000);
    }
    return NULL;
}
int main() {
    pthread_t readerThreads[NUM_READERS];
    pthread_t writerThreads[NUM_WRITERS];
    int i;

    sem_init(&mutex, 0, 1);
    sem_init(&writeBlock, 0, 1);

    for (i = 0; i < NUM_READERS; i++) {
        pthread_create(&readerThreads[i], NULL, reader, (void*)(intptr_t)i);
    }

    for (i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writerThreads[i], NULL, writer, (void*)(intptr_t)i);
    }

    sleep(5); 

    exitFlag = 1;
 
    for (i = 0; i < NUM_READERS; i++) {
        pthread_join(readerThreads[i], NULL);
    }
    for (i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writerThreads[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&writeBlock);

    return 0;
}