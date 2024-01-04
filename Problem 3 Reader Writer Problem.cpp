#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

int readers_count = 0;
sem_t mutex, write_sem, read_sem;
pthread_cond_t writer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reader_cond = PTHREAD_COND_INITIALIZER;

void *reader(void *arg) {
    int id = *(int *)arg;
    while (true) {
        sem_wait(&read_sem);
        sem_wait(&mutex);
        readers_count++;
        if (readers_count == 1) {
            sem_wait(&write_sem);
        }
        sem_post(&mutex);
        
        cout << "Reader " << id << " is reading..." << endl;

        sem_wait(&mutex);
        readers_count--;
        if (readers_count == 0) {
            sem_post(&write_sem);
        }
        sem_post(&mutex);
        
        sem_post(&read_sem);
        sleep(rand() % 3);
    }
    return NULL;
}

void *writer(void *arg) {
    int id = *(int *)arg;
    while (true) {
        sem_wait(&write_sem);
        
        cout << "Writer " << id << " is writing..." << endl;
        
        sem_post(&write_sem);
        pthread_cond_broadcast(&reader_cond);
        sleep(rand() % 3);
    }
    return NULL;
}


int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&write_sem, 0, 1);
    sem_init(&read_sem, 0, 1);

    pthread_t readers[5], writers[2];

    for (int i = 0; i < 5; ++i) {
        pthread_create(&readers[i], NULL, reader, new int(i));
    }

    for (int i = 0; i < 2; ++i) {
        pthread_create(&writers[i], NULL, writer, new int(i));
    }

    for (int i = 0; i < 5; ++i) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < 2; ++i) {
        pthread_join(writers[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&write_sem);
    sem_destroy(&read_sem);

    return 0;
}

