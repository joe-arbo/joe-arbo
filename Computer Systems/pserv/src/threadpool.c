/**
 * threadpool.h
 *
 * A work-stealing, fork-join thread pool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "threadpool.h"
#include "list.h"

static void* worker_thread(void * poolIn);
static struct future* find_from_global(struct thread_pool* pool);
static struct future* try_steal(struct thread_pool* pool);
static struct worker* get_worker_from_thread(struct thread_pool* pool, pthread_t t);
static bool no_work(struct thread_pool* pool);

static _Thread_local bool is_worker;

/*
* Enum to represent a future's possible states.
*/
enum state 
{ //static created warning
    NEW,
    IN_PROGRESS,
    DONE
};

/*
* Struct to bundle thread id with a "local" queue.
*/
struct worker 
{ //static created warning
    pthread_t thread; //thread id associated with worker
    struct list local_queue; //"local" queue for each worker
    struct list_elem elem;
};

/* 
 * Contains array of thread ids and global
 * queue of tasks.
 */
struct thread_pool 
{
    pthread_mutex_t lock;
    pthread_cond_t work_delivered;
    bool shutdown;
    int numthreads;
    struct list workers; //array of thread ids and local_queues bundled
    struct list global_queue; //global queue of tasks (futures)
}; 

/*
* Contains pointer to function to be called, any data passed
* to the function, result (when available), state
*/
struct future
{
    fork_join_task_t task; //the actual task
    enum state state; //state of future: should be "NEW" as default
    struct list_elem elem; //because it will be a part of a list
    void * data; //data to pass to task
    void * result; //result of completed task
    struct thread_pool * pool;
    pthread_cond_t result_done;
};

/* Create a new thread pool with no more than n threads. */
struct thread_pool * thread_pool_new(int nthreads)
{
    struct thread_pool* pool = malloc(sizeof (struct thread_pool));
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->work_delivered, NULL);
    pthread_mutex_lock(&pool->lock);

    pool->numthreads = nthreads;
    pool->shutdown = false;
    list_init(&pool->global_queue);
    list_init(&pool->workers);
    for (int i = 0; i < nthreads; ++i)
    {
        struct worker *worker_t = malloc(sizeof(struct worker));
        list_push_front(&pool->workers, &worker_t->elem);
        list_init(&worker_t->local_queue);
        pthread_create(&worker_t->thread, NULL, &worker_thread, pool); //can pass in theadpool to have it effectively global
    }
    is_worker = false;
    pthread_mutex_unlock(&pool->lock);

    return pool;
}

/*
* Logic for worker threads, including getting tasks from global or stealing,
* and then doing the function.
* Recieves a pointer to the threadpool to make it effectively "global"
*/
static void * worker_thread(void * poolIn)
{
    struct thread_pool * pool = (struct thread_pool *) poolIn;

    pthread_mutex_lock(&pool->lock); //FLAG
    struct worker * currentWorker = get_worker_from_thread(pool, pthread_self());
    is_worker = true;

	while(!pool->shutdown)
    {
        while(no_work(pool))
        {
            pthread_cond_wait(&pool->work_delivered, &pool->lock);
        }

        struct future * future = NULL;
        if (!list_empty(&currentWorker->local_queue))
        {
            struct list_elem * e = list_pop_front(&currentWorker->local_queue);
            future = list_entry(e, struct future, elem);
        }
        else if (!list_empty(&pool->global_queue))
            future = find_from_global(pool);
        else
            future = try_steal(pool);

        if (future)
        {
            future->state = IN_PROGRESS;
            pthread_mutex_unlock(&pool->lock);
            future->result = future->task(pool, future->data); //this is running the function
            pthread_mutex_lock(&pool->lock);
            future->state = DONE;
            pthread_cond_signal(&future->result_done);
        }
    }

	pthread_mutex_unlock(&pool->lock); //FLAG
    pthread_exit(NULL);
    return NULL;
}

/*
* To be called in worker_thread.
*/
static struct future * find_from_global(struct thread_pool* pool)
{
    while (!list_empty(&pool->global_queue)){
        struct list_elem* e = list_pop_front(&pool->global_queue);
        struct future* future = list_entry(e, struct future, elem);
        return future;
    }
    return NULL;
}

/**
 * will get the worker attached to the input thread
 */
static struct worker* get_worker_from_thread(struct thread_pool* pool, pthread_t t)
{
    struct list_elem * workerElem;
    struct worker * currentWorker = NULL;

    for (workerElem = list_begin(&pool->workers); 
         workerElem != list_end(&pool->workers); 
         workerElem = list_next(workerElem))
    {
        currentWorker = list_entry(workerElem, struct worker, elem);
        if (currentWorker->thread == t)
            return currentWorker;
    }
    return NULL;
}

/**
 * Will check to see if there is any work
 */
static bool no_work(struct thread_pool* pool)
{
    struct list_elem* e;
    struct worker* currentWorker;
    for (e = list_begin(&pool->workers); e != list_end(&pool->workers); e = list_next(e))
    {
        currentWorker = list_entry(e, struct worker, elem);
        if (!list_empty(&currentWorker->local_queue))
            return false;
    }
    currentWorker = get_worker_from_thread(pool, pthread_self());
    return list_empty(&pool->global_queue) && list_empty(&currentWorker->local_queue) && !pool->shutdown;
}

/*
* To be implemented after find_from_global works
* To be called in worker_thread.
*/
static struct future * try_steal(struct thread_pool* pool)
{
    struct list_elem* e;
    for (e = list_begin(&pool->workers); e != list_end(&pool->workers); e = list_next(e))
    {
        struct worker* currentWorker = list_entry(e, struct worker, elem);
        if (!list_empty(&currentWorker->local_queue))
        {
            struct list_elem* listElem = list_pop_back(&currentWorker->local_queue);
            struct future* future = list_entry(listElem, struct future, elem);
            if (future->state == NEW)
                return future;
        }
    }
    return NULL;
}

/* 
 * Shutdown this thread pool in an orderly fashion.  
 * Tasks that have been submitted but not executed may or
 * may not be executed.
 *
 * Deallocate the thread pool object before returning. 
 */
void thread_pool_shutdown_and_destroy(struct thread_pool * pool)
{
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->work_delivered);
    pthread_mutex_unlock(&pool->lock);

    //ensure all worker threads exit before freeing the pool
    struct list_elem *e;
    struct worker *f;
    for (e = list_begin (&pool->workers); e != list_end (&pool->workers); e = list_next (e))
    {
        f = list_entry (e, struct worker, elem);
        pthread_join(f->thread, NULL);
    }

    //free all workers
    while(!list_empty(&pool->workers))
    {
        e = list_pop_front(&pool->workers);
        f = list_entry(e, struct worker, elem);
        free(f);
    }

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->work_delivered);
    free(pool);
    return;
}

/* 
 * Submit a fork join task to the thread pool and return a
 * future.  The returned future can be used in future_get()
 * to obtain the result.
 * 'pool' - the pool to which to submit
 * 'task' - the task to be submitted.
 * 'data' - data to be passed to the task's function
 *
 * Returns a future representing this computation.
 */
struct future * thread_pool_submit(struct thread_pool *pool, fork_join_task_t task, void * data)
{
    pthread_mutex_lock(&pool->lock);
    struct future * future = malloc(sizeof(struct future));
    pthread_cond_init(&future->result_done, NULL);

    future->state = NEW;
    future->task = task;
    future->data = data;
    future->pool = pool;

    if (is_worker)
    {
        struct worker* currentWorker = get_worker_from_thread(pool, pthread_self());
        list_push_front(&currentWorker->local_queue, &future->elem);
    }
    else
        list_push_back(&pool->global_queue, &future->elem);
   
    pthread_cond_signal(&pool->work_delivered); //signal that new work has arrived
    pthread_mutex_unlock(&pool->lock);
    return future;
}

/* Make sure that the thread pool has completed the execution
 * of the fork join task this future represents.
 *
 * Returns the value returned by this task.
 */
void * future_get(struct future * future)
{
    pthread_mutex_lock(&future->pool->lock);

    if (future->state == NEW)
    {
        list_remove(&future->elem);
        future->state = IN_PROGRESS;
        pthread_mutex_unlock(&future->pool->lock);
        future->result = future->task(future->pool, future->data);
        pthread_mutex_lock(&future->pool->lock);
        future->state = DONE;
        pthread_cond_signal(&future->result_done);
    }
    else
    {
        while(future->state != DONE)
        {
            pthread_cond_wait(&future->result_done, &future->pool->lock); //wait for function to return
        }
    }

    pthread_mutex_unlock(&future->pool->lock);
    return future->result;
}

/* Deallocate this future.  Must be called after future_get() */
//DO NOT USE! We will not call this at all. Will be called outside of this file.
void future_free(struct future * future)
{
    //may need to free additional guts
    pthread_cond_destroy(&future->result_done);
    free(future);
}
