#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>  // for sleep function used inside code to add a bit of waiting time
#include <vector>  // for storing eating sequence

using namespace std;

const int NUM_PHILOSOPHERS = 5;
const int EAT_LIMIT = 1; // Considering each philosopher can eat only 1 times

pthread_mutex_t forks[NUM_PHILOSOPHERS];
sem_t semaphores[NUM_PHILOSOPHERS];
int eat_count[NUM_PHILOSOPHERS] = {0};
vector<int> eating_sequence;

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;

    while (eat_count[id] < EAT_LIMIT) { // Checking eat limit so that we can stop the code and get a sequence at the end.
        // Thinking
        cout << "Philosopher " << id << " is thinking." << endl;
        sleep(rand() % 3);

        // Picking up forks
        cout << "Philosopher " << id << " is picking up forks." << endl;
        sem_wait(&semaphores[left_fork]);
        sem_wait(&semaphores[right_fork]);

        // Eating
        cout << "Philosopher " << id << " is eating." << endl;
        eat_count[id]++;
        eating_sequence.push_back(id); // Store the eating sequence
        sleep(rand() % 2);

        // Putting down forks
        cout << "Philosopher " << id << " is putting down forks." << endl;
        sem_post(&semaphores[left_fork]);
        sem_post(&semaphores[right_fork]);
    }

    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // Initializing mutexes and semaphores
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_mutex_init(&forks[i], NULL);
        sem_init(&semaphores[i], 0, 1); // Binary semaphore
    }

    // Creating philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    // Joining philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_join(philosophers[i], NULL);
    }

    // Destroying mutexes and semaphores
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_mutex_destroy(&forks[i]);
        sem_destroy(&semaphores[i]);
    }

    // Printing eating sequence summary
    cout << "\nEating Sequence: ";
    for (int id : eating_sequence) {
        cout << id << " ";
    }
    cout << endl;

    return 0;
}

