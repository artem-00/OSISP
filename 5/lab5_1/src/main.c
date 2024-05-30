#include "ring.h"
#include "routines.h"

#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdio.h>

#define DEFAULT_RING_SIZE 10
#define MAX_THREADS_COUNT 50

ring_t* ring;

int main()
{
    sem_unlink("/producer");
    sem_unlink("/consumer");
    sem_unlink("/mutex");

    sem_t* producer = sem_open("/producer", O_CREAT, 0644, 1);
    sem_t* consumer = sem_open("/consumer", O_CREAT, 0644, 1);
    sem_t* mutex    = sem_open("/mutex", O_CREAT, 0644, 1);

    pthread_t threads[MAX_THREADS_COUNT];
    size_t producer_count = 0;
    size_t consumer_count = 0;
    size_t threads_count  = 0;
    char input[2];

    ring       = (ring_t*)calloc(1, sizeof(ring_t));
    ring->size = DEFAULT_RING_SIZE;

    while (1)
    {
        scanf("%s", input);

        switch (input[0])
        {
        case 'p':
        {
            pthread_t producer_thread_id;
            pthread_create(&producer_thread_id, NULL, producer_routine, NULL);
            threads[threads_count++] = producer_thread_id;
            producer_count++;
            break;
        }        
        case 'c':
        {
            pthread_t consumer_thread_id;
            pthread_create(&consumer_thread_id, NULL, consumer_routine, NULL);
            threads[threads_count++] = consumer_thread_id;
            consumer_count++;
            break;
        }
        case 's':
        {
            printf("\n=====================\n");
            printf("Added: %ld\nGetted: %ld\nProducers count: %ld\nConsumers count: %ld\nCurrent size: %ld\n", 
                                                                                ring->added, 
                                                                                ring->deleted, 
                                                                                producer_count, 
                                                                                consumer_count,
                                                                                ring->cur);
            printf("=====================\n\n");                                                          
            break;
        }
        case '+':
        {
            sem_wait(mutex);
            ring->size++;
            sem_post(mutex);
            break;
        }
        case '-':
        {
            sem_wait(mutex);
            if (ring->size > 0) 
            {
                ring->size--;
                if (ring->cur > ring->size)
                {
                    pop(&ring->head, &ring->tail);
                    ring->cur--;
                } 
            }
            else printf("\nRING IS EMPTY\n");
            sem_post(mutex);
            break;
        }
        case 'q':
        {
            while (threads_count != 0)
            {
                pthread_kill(threads[threads_count - 1], SIGUSR1);
                pthread_join(threads[threads_count - 1], NULL);
                threads_count--;
            }

            sem_close(producer);
            sem_close(consumer);
            sem_close(mutex);
            
            sem_unlink("/producer");
            sem_unlink("/consumer");
            sem_unlink("/mutex");
            
            return 0;
        }
        default:
            break;
        }
    }
    return 0;
}