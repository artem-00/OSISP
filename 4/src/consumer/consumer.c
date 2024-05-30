#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>

#include "../common.h"
#include "../ring.h"

bool is_true = true;

void signal_handler(int signo) {
    if (signo != SIGUSR1) {
        return;
    }

    is_true = false;
}

int main(ATTR_UNUSED int argc, char** argv) {
    signal(SIGUSR1, signal_handler);

    sem_t* sem_prod = sem_open(SEM_PRODUCER, 0);
    sem_t* sem_cons = sem_open(SEM_CONSUMER, 0);
    sem_t* sem_mutex = sem_open(SEM_MUTEX, 0);

    Ring* ring = (Ring*)shmat(atoi(argv[0]), NULL, 0);

    do {
        sem_wait(sem_prod);
        sem_wait(sem_mutex);

        sleep(2);
        
        Message message = get_data(ring);
        ring->removed_messages_counter++;

        sem_post(sem_mutex);
        sem_post(sem_cons);

        printf("-------------Consumer--------------\n");
        printf("Message data %s\n", message.data);
        printf("Total removed counter %ld\n", ring->removed_messages_counter);
        printf("Consumer pid %d\n", getpid());
        printf("-----------------------------------\n");
    } while (is_true);

    shmdt(ring);

    exit(0);
}