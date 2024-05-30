#include "ring.h"

#define __USE_POSIX

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>

#include "common.h"

pid_t* childs = NULL;
size_t total_size = 0;

int main() {
    //синхронизация процессов-потребителей 
    sem_unlink(SEM_PRODUCER);
    sem_unlink(SEM_CONSUMER);
    sem_unlink(SEM_MUTEX);
    //создание семафоров
    sem_t* sem_prod = sem_open(SEM_PRODUCER, O_CREAT, 0664, 0);
    sem_t* sem_cons = sem_open(SEM_CONSUMER, O_CREAT, 0664, RING_SIZE);
    sem_t* sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0664, 1);
    
    //инициализация кольца
    int shmid = shmget(0, sizeof(Ring), 0777);  //доступ к разделяемой памяти 
    Ring* ring = shmat(shmid, NULL, 0); //индикатор памяти к которой получен доступ 

    init_ring(ring);

    char str_shmid[255];
    snprintf(str_shmid, 255, "%d", shmid);

    char choice;
    while (choice != 'q') {
        choice = getchar();
        getchar();

        switch (choice) {
        case 'p': {
            pid_t prod_pid = fork();
            if (prod_pid == 0) {
                char* args[] = {str_shmid, NULL};

                execv("/home/artem/Documents/OSISP/temp/OSISP/4/bin/producer", args);

                printf("failed producer\n");
            } else if (prod_pid > 0) {
                childs = (pid_t*)realloc(childs, (total_size + 1) * sizeof(pid_t));
                childs[total_size++] = prod_pid;
            }

            break;
        } 
        case 'c': {
            pid_t prod_pid = fork();
            if (prod_pid == 0) {
                char* args[] = {str_shmid, NULL};

                execv("/home/artem/Documents/OSISP/temp/OSISP/4/bin/consumer", args);

                printf("failed consumer\n");
            } else if (prod_pid > 0) {
                childs = (pid_t*)realloc(childs, (total_size + 1) * sizeof(pid_t));
                childs[total_size++] = prod_pid;
            }

            break;
        }
        default:
            break;
        }
    }

    for (size_t i = 0; i < total_size; ++i) {
        kill(childs[i], SIGUSR1);
    }            

    sem_unlink(SEM_PRODUCER);
    sem_unlink(SEM_CONSUMER);
    sem_unlink(SEM_MUTEX);

    sem_close(sem_cons);
    sem_close(sem_prod);
    sem_close(sem_mutex);

    return 0;
}