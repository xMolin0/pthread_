/*
 * axpy: Y += a*X[N]
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/timeb.h>
#include <pthread.h>


/* read timer in second */
double read_timer() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time + (double) tm.millitm / 1000.0;
}

/* read timer in ms */
double read_timer_ms() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time * 1000.0 + (double) tm.millitm;
}

#define REAL float
#define VECTOR_LENGTH 102400

typedef struct {
    int thread_id;
    int start;      // Starting index in the arrays
    int end;        // Ending index (exclusive)
    REAL *Y;        // Pointer to Y array
    REAL *X;        // Pointer to X array
    REAL a;         // Scalar value
} thread_data_t;

/* initialize a vector with random floating point numbers */
void init(REAL A[], int N) {
    int i;
    for (i = 0; i < N; i++) {
        A[i] = (double) drand48();
    }
}

void axpy_kernel(int N, REAL *Y, REAL *X, REAL a) {
    int i;
    for (i = 0; i < N; ++i)
        Y[i] += a * X[i];
}

void* axpy_thread_func(void* args) {
    thread_data_t* targs = (thread_data_t*) args;

    for (int i = targs->start; i < targs->end; i++) {
        targs->Y[i] += targs->a * targs->X[i];
    }

    return NULL;
}

/**
 * Your implementation of pthread version of axpy computation using loop chunking and worksharing
 * by dividing the total number of iterations amount by the num_threads for parallel computing.
 */
void axpy_kernel_threading(int N, REAL *Y, REAL *X, REAL a, int num_threads) {
    pthread_t threads[num_threads];
    thread_data_t thread_data[num_threads];

    int chunk_size = N / num_threads;
    int remainder = N % num_threads;

    int current_start = 0;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start = current_start;

        // Give first 'remainder' threads one extra iteration
        int my_chunk = chunk_size + (i < remainder ? 1 : 0);
        thread_data[i].end = current_start + my_chunk;

        thread_data[i].Y = Y;
        thread_data[i].X = X;
        thread_data[i].a = a;

        pthread_create(&threads[i], NULL, axpy_thread_func, &thread_data[i]);

        current_start += my_chunk;
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char *argv[]) {
    int N = VECTOR_LENGTH;
    int num_threads = 4;
    if (argc < 2) {
        fprintf(stderr, "Usage: axpy <n> [<num_threads>] (The default of n is %d, the default num_threads is %d)\n", N, num_threads);
    } else if (argc == 2) N = atoi(argv[1]);
    else {
        N = atoi(argv[1]);
        num_threads = atoi(argv[2]);
    }
    REAL *X = (REAL*)malloc(sizeof(REAL)*N);
    REAL *Y = (REAL*)malloc(sizeof(REAL)*N);

    srand48((1 << 12));
    init(X, N);
    init(Y, N);
    int num_runs = 10; int i;
    REAL a = 0.1234;
    /* example run */
    double elapsed; /* for timing */
    elapsed = read_timer();
    for (i=0; i<num_runs; i++) axpy_kernel(N, Y, X, a);
    elapsed = (read_timer() - elapsed)/num_runs;
    
    double elapsed2; /* for timing */
    elapsed2 = read_timer();
    for (i=0; i<num_runs; i++) axpy_kernel_threading(N, Y, X, a, num_threads);
    elapsed2 = (read_timer() - elapsed2)/num_runs;
    printf("======================================================================================================\n");
    printf("\tAXPY %d numbers, serial and threading\n", N);
    printf("------------------------------------------------------------------------------------------------------\n");
    printf("Performance:\t\tRuntime (ms)\t MFLOPS \n");
    printf("------------------------------------------------------------------------------------------------------\n");
    printf("AXPY-serial:\t\t%4f\t%4f\n", elapsed * 1.0e3, 2*N / (1.0e6 * elapsed));
    printf("AXPY-%d threads:\t\t%4f\t%4f\n", num_threads, elapsed2 * 1.0e3, 2*N / (1.0e6 * elapsed2));
    return 0;
}
