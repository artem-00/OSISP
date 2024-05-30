#include <fcntl.h>
#include <semaphore.h>
#include <stdnoreturn.h>
#include <stdio.h>
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

Message generate_new_message() {
     Message message = {
        .data = {0},
        .hash = 0,
        .size = 0,
        .type = 0
    };

    size_t size = 0;

    while (size == 0) {
        size = rand() % DATA_MAX_LEN;
    }

    if (size == DATA_MAX_LEN) {
        size = 0;
        message.size = DATA_MAX_LEN;
    } else {
        message.size = ((size + 3 ) / 4) * 4;
    }

    for (int i = 0; i < message.size; ++i) {
        message.data[i] = rand() % DATA_MAX_LEN;
        message.hash += message.data[i];
    }

    return message;
}

int main(ATTR_UNUSED int argc, char** argv) {
    signal(SIGUSR1, signal_handler);

    srand(0);

    sem_t* sem_prod = sem_open(SEM_PRODUCER, 0);
    sem_t* sem_cons = sem_open(SEM_CONSUMER, 0);
    sem_t* sem_mutex = sem_open(SEM_MUTEX, 0);

    printf("PROD running\n");

    Ring* ring = (Ring*)shmat(atoi(argv[0]), NULL, 0);

    do {
        Message message = generate_new_message();

        sem_wait(sem_cons);
        sem_wait(sem_mutex);

        sleep(1);
        
        push_data(ring, &message);
        ring->added_messages_counter++;

        sem_post(sem_mutex);
        sem_post(sem_prod);

        printf("-------------Producer--------------\n");
        printf("Total added counter %ld\n", ring->added_messages_counter);
        printf("Producer pid %d\n", getpid());
        printf("-------------------------------------\n");
    } while (is_true);

    shmdt(ring);

    exit(0);
}