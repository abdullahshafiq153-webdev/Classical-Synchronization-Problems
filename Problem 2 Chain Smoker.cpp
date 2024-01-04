#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

sem_t tobacco, paper, matches;
pthread_mutex_t tableMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t agentCond = PTHREAD_COND_INITIALIZER;

int tableContents = -1; // -1: Empty, 0: Tobacco & Paper, 1: Tobacco & Matches, 2: Paper & Matches

void *agent(void *arg) {
    while (true) {
        pthread_mutex_lock(&tableMutex);
        int random = rand() % 3;
        switch (random) {
            case 0:
                sem_post(&tobacco);
                sem_post(&paper);
                tableContents = 0;
                cout << "Agent placed tobacco and paper on the table." << endl;
                break;
            case 1:
                sem_post(&tobacco);
                sem_post(&matches);
                tableContents = 1;
                cout << "Agent placed tobacco and matches on the table." << endl;
                break;
            case 2:
                sem_post(&paper);
                sem_post(&matches);
                tableContents = 2;
                cout << "Agent placed paper and matches on the table." << endl;
                break;
        }
        pthread_cond_broadcast(&agentCond); // Signal waiting smokers
        pthread_mutex_unlock(&tableMutex);
        sleep(1);
    }
    return NULL;
}

void *smokerTobacco(void *arg) {
    while (true) {
        pthread_mutex_lock(&tableMutex);
        while (tableContents != 2) {
            pthread_cond_wait(&agentCond, &tableMutex);
        }
        cout << "Smoker with tobacco is smoking..." << endl << endl;
        tableContents = -1;
        pthread_mutex_unlock(&tableMutex);
    }
    return NULL;
}

void *smokerPaper(void *arg) {
    while (true) {
        pthread_mutex_lock(&tableMutex);
        while (tableContents != 1) {
            pthread_cond_wait(&agentCond, &tableMutex);
        }
        cout << "Smoker with paper is smoking..." << endl << endl;
        tableContents = -1;
        pthread_mutex_unlock(&tableMutex);
    }
    return NULL;
}

void *smokerMatches(void *arg) {
    while (true) {
        pthread_mutex_lock(&tableMutex);
        while (tableContents != 0) {
            pthread_cond_wait(&agentCond, &tableMutex);
        }
        cout << "Smoker with matches is smoking..." << endl << endl;
        tableContents = -1;
        pthread_mutex_unlock(&tableMutex);
    }
    return NULL;
}

int main() {
    sem_init(&tobacco, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&matches, 0, 0);

    pthread_t agentThread, smokerTobaccoThread, smokerPaperThread, smokerMatchesThread;

    pthread_create(&agentThread, NULL, agent, NULL);
    pthread_create(&smokerTobaccoThread, NULL, smokerTobacco, NULL);
    pthread_create(&smokerPaperThread, NULL, smokerPaper, NULL);
    pthread_create(&smokerMatchesThread, NULL, smokerMatches, NULL);

    pthread_join(agentThread, NULL);
    pthread_join(smokerTobaccoThread, NULL);
    pthread_join(smokerPaperThread, NULL);
    pthread_join(smokerMatchesThread, NULL);

    sem_destroy(&tobacco);
    sem_destroy(&paper);
    sem_destroy(&matches);

    return 0;
}

