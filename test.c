#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "queue.c"

#define NUM_OPERATIONS 10
#define MAX_SIZE 1000
#define NUM_THREADS_CONC 100
#define NUM_THREADS 50
#define SECOND_IN_NANOSECONDS 1000000000

int dequeue_with_sleep(void *arg);
int enqueueItems(void *arg);
int enqueue_thread(void *arg);
int dequeue_thread(void *arg);
int consumer_thread(void *arg);
int producer_thread(void *arg);

void test_destroyQueue()
{
    printf("=== Testing destroyQueue ===\n");

    initQueue();

    destroyQueue();

    printf("destroyQueue test passed.\n");
}

void test_enqueue_dequeue()
{
    printf("=== Testing enqueue and dequeue ===\n");

    initQueue();

    int items[] = {1, 2, 3, 4, 5};
    size_t num_items = sizeof(items) / sizeof(items[0]);

    // Enqueue items
    for (size_t i = 0; i < num_items; i++)
    {
        enqueue(&items[i]);
    }

    // Dequeue items and check the order
    for (size_t i = 0; i < num_items; i++)
    {
        int *item = (int *)dequeue();
        printf("Dequeued: %d\n", *item);
        assert(*item == items[i]);
    }

    // Queue should be empty
    assert(size() == 0);

    destroyQueue();

    printf("enqueue and dequeue test passed.\n");
}

void test_tryDequeue()
{
    printf("=== Testing tryDequeue ===\n");

    initQueue();

    int items[] = {1, 2, 3, 4, 5};
    size_t num_items = sizeof(items) / sizeof(items[0]);

    // Try dequeueing when the queue is empty
    void *item;
    assert(!tryDequeue(&item));

    // Enqueue items
    for (size_t i = 0; i < num_items; i++)
    {
        enqueue(&items[i]);
    }

    // Try dequeueing items and check the order
    for (size_t i = 0; i < num_items; i++)
    {
        assert(tryDequeue(&item));
        printf("Dequeued: %d\n", *(int *)item);
        assert(*(int *)item == items[i]);
    }

    // Queue should be empty
    assert(size() == 0);

    destroyQueue();

    printf("tryDequeue test passed.\n");
}

void test_size()
{
    printf("=== Testing size ===\n");

    initQueue();

    int items[] = {1, 2, 3};
    size_t num_items = sizeof(items) / sizeof(items[0]);

    // Enqueue items
    for (size_t i = 0; i < num_items; i++)
    {
        enqueue(&items[i]);
    }

    // Check size
    assert(size() == num_items);

    // Perform enqueue and dequeue operations and check if size updates correctly
    // ...

    destroyQueue();

    printf("size test passed.\n");
}

int dequeue_with_wait(void *arg)
{
    struct timespec *sleep_time = (struct timespec *)arg;

    void *item;
    while (1)
    {
        if (tryDequeue(&item))
        {
            // Item dequeued successfully
            int value = *(int *)item;
            printf("Dequeued item: %d\n", value);
            return value;
        }
        else
        {
            // Queue is empty, sleep for the specified time
            thrd_sleep(sleep_time, NULL);
        }
    }
}

void test_waiting()
{
    initQueue();

    int items[] = {1, 2, 3};
    size_t num_items = sizeof(items) / sizeof(items[0]);

    // Enqueue items
    for (size_t i = 0; i < num_items; i++)
    {
        enqueue(&items[i]);
    }

    // Create threads that will wait for items in the queue
    thrd_t threads[3];
    struct timespec sleep_time = {5, 0}; // Sleep time of 5 seconds

    for (int i = 0; i < 3; i++)
    {
        thrd_create(&threads[i], (int (*)(void *))dequeue_with_wait, &sleep_time);
    }

    // Wait for all threads to finish
    int values[3];
    for (int i = 0; i < 3; i++)
    {
        thrd_join(threads[i], &values[i]);
    }

    // Queue should be empty
    assert(size() == 0);
    // All items should have been enqueued and dequeued
    assert(visited() == num_items);
    // No threads should be waiting
    assert(waiting() == 0);

    destroyQueue();
}

void test_basic_concurrent_enqueue_dequeue()
{
    printf("=== Testing basic concurrent enqueue and dequeue ===\n");

    initQueue();

    // Number of items to enqueue
    int numItems = 100;
    int half = numItems / 2;
    // Enqueue items in a separate thread
    thrd_t enqueueThread_a;
    thrd_t enqueueThread_b;
    thrd_create(&enqueueThread_a, (int (*)(void *))enqueueItems, &half);
    thrd_create(&enqueueThread_b, (int (*)(void *))enqueueItems, &half);

    // Dequeue items in the main thread
    for (int i = 0; i < numItems; i++)
    {
        int *item = (int *)dequeue();
        printf("Dequeued item: %d\n", *item);
        free(item);
    }

    thrd_join(enqueueThread_a, NULL);
    thrd_join(enqueueThread_b, NULL);

    destroyQueue();

    printf("Basic concurrent enqueue and dequeue test passed.\n");
}

// Thread function to enqueue items
int enqueueItems(void *arg)
{
    int numItems = *(int *)arg;
    for (int i = 0; i < numItems; i++)
    {
        // printf("%d\n", i);
        int *item = (int *)malloc(sizeof(int));
        *item = i + 1;
        // printf("%d\n", *item);
        enqueue((void *)item);
        printf("Enqueued item: %d\n", i + 1);
    }
    thrd_exit(0);
}

void test_enqueue_tryDequeue()
{
    printf("=== Testing enqueue and tryDequeue ===\n");

    initQueue();

    // Enqueue items
    int item1 = 1;
    int item2 = 2;
    int item3 = 3;
    enqueue(&item1);
    enqueue(&item2);
    enqueue(&item3);

    // Try dequeueing items and check the order
    void *item;
    assert(tryDequeue(&item));
    printf("Dequeued item: %d\n", *(int *)item);
    assert(*(int *)item == item1);

    assert(tryDequeue(&item));
    printf("Dequeued item: %d\n", *(int *)item);
    assert(*(int *)item == item2);

    assert(tryDequeue(&item));
    printf("Dequeued item: %d\n", *(int *)item);
    assert(*(int *)item == item3);

    // Try dequeue from an empty queue
    assert(!tryDequeue(&item));

    destroyQueue();

    printf("enqueue and tryDequeue test passed.\n");
}

void test_enqueue_dequeue_with_sleep()
{
    printf("=== Testing enqueue and dequeue with sleep time ===\n");

    initQueue();

    // Enqueue items
    int item1 = 1;
    int item2 = 2;
    int item3 = 3;
    enqueue(&item1);
    enqueue(&item2);
    enqueue(&item3);

    // Create multiple threads to concurrently dequeue items with a delay between each dequeue operation
    thrd_t threads[3];
    for (int i = 0; i < 3; i++)
    {
        thrd_create(&threads[i], dequeue_with_sleep, NULL);
    }

    // Wait for all threads to finish
    for (int i = 0; i < 3; i++)
    {
        thrd_join(threads[i], NULL);
    }

    // Queue should be empty
    assert(size() == 0);

    destroyQueue();

    printf("enqueue and dequeue with sleep time test passed.\n");
}

int dequeue_with_sleep(void *arg)
{
    int sleep_time = (rand() % 6 + 5) * 1000; // Random sleep time between 5000 and 10000 milliseconds

    thrd_sleep(&(struct timespec){sleep_time / 1000, (sleep_time % 1000) * 1000000}, NULL);

    int *item = (int *)dequeue();
    printf("Dequeued item with sleep: %d\n", *item);

    return 0;
}

void test_fifo_order()
{
    printf("=== Testing FIFO order ===\n");

    initQueue();

    thrd_t consumer_threads[NUM_THREADS];
    int dequeue_order[NUM_THREADS];

    // Create consumer threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        dequeue_order[i] = -1; // Initialize the dequeue order
        thrd_create(&consumer_threads[i], consumer_thread, &dequeue_order[i]);
        thrd_sleep(
            &(const struct timespec){.tv_nsec = 0.005 * SECOND_IN_NANOSECONDS},
            NULL);
    }

    // Create producer thread
    thrd_t producer_thread_handle;
    thrd_create(&producer_thread_handle, producer_thread, NULL);

    // Wait for the producer thread to finish
    thrd_join(producer_thread_handle, NULL);

    // Wait for all consumer threads to finish dequeuing
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thrd_join(consumer_threads[i], NULL);
    }

    // Verify FIFO order
    for (int i = 0; i < NUM_THREADS; i++)
    {
        printf("Thread %d dequeued item %d\n", i, dequeue_order[i]);
        assert(dequeue_order[i] == i + 1);
    }

    destroyQueue();

    printf("FIFO order test passed.\n");
}

int consumer_thread(void *arg)
{
    int *dequeue_order = (int *)arg;

    void *item = dequeue();

    *dequeue_order = *(int *)item;
    free(item);

    return 0;
}

int producer_thread(void *arg)
{
    (void)arg;

    // Enqueue at least NUM_THREADS items
    for (int i = 0; i < NUM_THREADS; i++)
    {
        int *item = malloc(sizeof(int));
        *item = i + 1;
        enqueue(item);
    }

    return 0;
}
void test_multiconcurrent_enqueue_dequeue()
{
    printf("=== Testing multiconcurrent enqueue and dequeue ===\n");

    initQueue();

    thrd_t enqueueThreads[NUM_THREADS_CONC];
    thrd_t dequeueThreads[NUM_THREADS_CONC];

    // Create threads for dequeueing
    for (int i = 0; i < NUM_THREADS_CONC; i++)
    {
        thrd_create(&dequeueThreads[i], (int (*)(void *))dequeue_thread, NULL);
    }

    // Create threads for enqueueing
    for (int i = 0; i < NUM_THREADS_CONC; i++)
    {
        thrd_create(&enqueueThreads[i], (int (*)(void *))enqueue_thread, NULL);
    }
    // Wait for enqueueing threads to finish
    for (int i = 0; i < NUM_THREADS_CONC; i++)
    {
        thrd_join(enqueueThreads[i], NULL);
    }

    // Wait for dequeueing threads to finish
    for (int i = 0; i < NUM_THREADS_CONC; i++)
    {
        thrd_join(dequeueThreads[i], NULL);
    }

    // Queue should be empty
    assert(size() == 0);
    // All items should have been dequeued
    assert(visited() == NUM_THREADS_CONC);
    // No threads should be waiting
    assert(waiting() == 0);

    destroyQueue();
    printf("Multiconcurrent enqueue and dequeue test passed.\n");
}

int enqueue_thread(void *arg)
{
    unsigned long *item = malloc(sizeof(unsigned long));
    *item = thrd_current(); // Set item value to thread index

    enqueue(item);
    printf("Thread %lx enqueued item: %lx\n", thrd_current(), thrd_current());

    return 0;
}

int dequeue_thread(void *arg)
{
    unsigned long *item = (unsigned long *)dequeue();
    printf("Thread %lx dequeued item: %lx\n", thrd_current(), thrd_current());
    free(item);

    return 0;
}

void test_edge_cases()
{
    printf("=== Testing edge cases ===\n");

    initQueue();

    // Dequeue from an empty queue - Should block until an item is enqueued
    // FIXME: comment this in, make sure it blocks, then comment it back out
    // int *item = (int *)dequeue();
    // assert(item == NULL);
    // printf("Dequeue from an empty queue - Assertion failed: Expected NULL\n");

    destroyQueue();

    printf("edge cases test passed.\n");
}

void test_mixed_operations()
{
    printf("=== Testing mixed operations ===\n");

    initQueue();

    // Enqueue items
    int item1 = 1;
    enqueue(&item1);
    printf("Enqueued item: %d\n", item1);

    // Try dequeue
    void *tryDequeueItem;
    if (tryDequeue(&tryDequeueItem))
    {
        int *dequeuedItem = (int *)tryDequeueItem;
        printf("Dequeued item: %d\n", *dequeuedItem);
    }

    // Enqueue more items
    int item2 = 2;
    enqueue(&item2);
    printf("Enqueued item: %d\n", item2);

    int item3 = 3;
    enqueue(&item3);
    printf("Enqueued item: %d\n", item3);

    // Dequeue an item
    int *dequeuedItem = (int *)dequeue();
    printf("Dequeued item: %d\n", *dequeuedItem);

    // Enqueue another item
    int item4 = 4;
    enqueue(&item4);
    printf("Enqueued item: %d\n", item4);

    // Try dequeue multiple times
    for (int i = 0; i < 3; i++)
    {
        if (tryDequeue(&tryDequeueItem))
        {
            int *dequeuedItem = (int *)tryDequeueItem;
            printf("Dequeued item: %d\n", *dequeuedItem);
        }
    }

    // Enqueue additional items
    int item5 = 5;
    enqueue(&item5);
    printf("Enqueued item: %d\n", item5);

    int item6 = 6;
    enqueue(&item6);
    printf("Enqueued item: %d\n", item6);

    // Dequeue remaining items
    while (size() > 0)
    {
        int *dequeuedItem = (int *)dequeue();
        printf("Dequeued item: %d\n", *dequeuedItem);
    }

    destroyQueue();

    printf("mixed operations test passed.\n");
}

int main()
{
    test_destroyQueue();
    test_enqueue_dequeue();
    test_tryDequeue();
    test_size();
    test_waiting();
    test_basic_concurrent_enqueue_dequeue();
    test_fifo_order();
    test_multiconcurrent_enqueue_dequeue();
    test_enqueue_tryDequeue();
    test_enqueue_dequeue_with_sleep();
    test_edge_cases();
    test_mixed_operations();

    return 0;
}
