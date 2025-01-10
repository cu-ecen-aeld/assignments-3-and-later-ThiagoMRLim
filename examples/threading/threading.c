#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
//#define DEBUG_LOG(msg,...)
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    if (!thread_func_args) {
        ERROR_LOG("Thread data is NULL");
        pthread_exit(NULL);
    }

    // Wait before attempting to obtain the mutex
    usleep(thread_func_args->wait_to_obtain_ms * 1000);

    // Attempt to lock the mutex
    if (pthread_mutex_lock(thread_func_args->mutex) != 0) {
        ERROR_LOG("Failed to lock mutex");
        thread_func_args->thread_complete_success = false;
        pthread_exit(thread_param);
    }

    DEBUG_LOG("Mutex locked by thread");

    // Wait while holding the mutex
    usleep(thread_func_args->wait_to_release_ms * 1000);

    // Release the mutex
    if (pthread_mutex_unlock(thread_func_args->mutex) != 0) {
        ERROR_LOG("Failed to unlock mutex");
        thread_func_args->thread_complete_success = false;
        pthread_exit(thread_param);
    }

    DEBUG_LOG("Mutex unlocked by thread");

    // Mark the thread as successful
    thread_func_args->thread_complete_success = true;

    pthread_exit(thread_param);
}

bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    if (!thread || !mutex) {
        ERROR_LOG("Invalid arguments to start_thread_obtaining_mutex");
        return false;
    }

    // Allocate memory for thread_data structure
    struct thread_data* data = malloc(sizeof(struct thread_data));
    if (!data) {
        ERROR_LOG("Failed to allocate memory for thread_data");
        return false;
    }

    // Initialize thread_data fields
    data->thread_complete_success = false;
    data->mutex = mutex;
    data->wait_to_obtain_ms = wait_to_obtain_ms;
    data->wait_to_release_ms = wait_to_release_ms;

    // Create the thread
    if (pthread_create(thread, NULL, threadfunc, data) != 0) {
        ERROR_LOG("Failed to create thread");
        free(data);
        return false;
    }

    DEBUG_LOG("Thread created successfully");
    return true;
}
