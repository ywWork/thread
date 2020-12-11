#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>
// #include<ostream>
#include<functional>

#include"thread.h"

// #define THREAD
// #define XTHREAD
#define XTHREAD_D
// #define RAW_ABT_THREAD

using namespace std;

#ifdef RAW_ABT_THREAD 
ABT_pool *pools;
#endif

typedef struct {
    int n;
    int ret;
} fibonacci_arg_t;

std::mutex mtx_global;

#ifdef THREAD
void fibonacci_thread(void *arg)
{
    int n = ((fibonacci_arg_t *)arg)->n;
    int *p_ret = &((fibonacci_arg_t *)arg)->ret;

    if (n <= 1) {
        *p_ret = 1;
    } 
	else {
        fibonacci_arg_t child1_arg = {n - 1, 0};
        fibonacci_arg_t child2_arg = {n - 2, 0};

		std::thread threads (fibonacci_thread, &child1_arg);

        /* Calculate fib(n - 2).  We do not create another ULT. */
        fibonacci_thread(&child2_arg);

		threads.join();

        *p_ret = child1_arg.ret + child2_arg.ret;
    }
}
#endif

#ifdef RAW_ABT_THREAD 
void fibonacci_raw(void *arg)
{
    int n = ((fibonacci_arg_t *)arg)->n;
    int *p_ret = &((fibonacci_arg_t *)arg)->ret;

    if (n <= 1) {
        *p_ret = 1;
    } 
	else {
        fibonacci_arg_t child1_arg = {n - 1, 0};
        fibonacci_arg_t child2_arg = {n - 2, 0};

        int rank;
        ABT_xstream_self_rank(&rank);
        ABT_pool target_pool = pools[rank];
        ABT_thread child1;
        /* Calculate fib(n - 1). */
        ABT_thread_create(target_pool, fibonacci_raw, &child1_arg,
                          ABT_THREAD_ATTR_NULL, &child1);
        /* Calculate fib(n - 2).  We do not create another ULT. */
        fibonacci_raw(&child2_arg);
        ABT_thread_free(&child1);
        *p_ret = child1_arg.ret + child2_arg.ret;
    }
}
#endif

#ifdef XTHREAD
void fibonacci_xthread(void *arg)
{
    int n = ((fibonacci_arg_t *)arg)->n;
    int *p_ret = &((fibonacci_arg_t *)arg)->ret;

    if (n <= 1) {
        *p_ret = 1;
    } 
	else {
        fibonacci_arg_t child1_arg = {n - 1, 0};
        fibonacci_arg_t child2_arg = {n - 2, 0};

		stdx::thread threads (fibonacci_xthread, &child1_arg);

        /* Calculate fib(n - 2).  We do not create another ULT. */
        fibonacci_xthread(&child2_arg);

		threads.join();

        *p_ret = child1_arg.ret + child2_arg.ret;
    }
}
#endif

#ifdef XTHREAD_D
void fibonacci_xthread_d(void *arg)
{
    int n = ((fibonacci_arg_t *)arg)->n;
    int *p_ret = &((fibonacci_arg_t *)arg)->ret;

    if (n <= 1) {
        *p_ret = 1;
    } 
	else {
        fibonacci_arg_t child1_arg = {n - 1, 0};
        fibonacci_arg_t child2_arg = {n - 2, 0};

		stdx::thread_d threads (fibonacci_xthread_d, &child1_arg);

        /* Calculate fib(n - 2).  We do not create another ULT. */
        fibonacci_xthread_d(&child2_arg);

		threads.wait();

        *p_ret = child1_arg.ret + child2_arg.ret;
    }
}
#endif

int fibonacci_seq(int n)
{
    if (n <= 1) {
        return 1;
    } else {
        int i;
        int fib_i1 = 1; /* Value of fib(i - 1) */
        int fib_i2 = 1; /* Value of fib(i - 2) */
        for (i = 3; i <= n; i++) {
            int tmp = fib_i1;
            fib_i1 = fib_i1 + fib_i2;
            fib_i2 = tmp;
        }
        return fib_i1 + fib_i2;
    }
}


class Half 
{
	public:
		Half(){}; 
		~Half(){}; 

		int operator() (int x) 
		{
			cout << x /2 << endl;
			return x/ 2;
		}
};


double test (int a, double b)  
{
	cout << "a + b " << a + b << endl;
	return a + b;
}


int main (int argc, char * argv[])
{

	int n = 13;

	/* Read arguments. */
	while (1) {
		int opt = getopt(argc, argv, "he:n:");
		if (opt == -1)
			break;
		switch (opt) {
			case 'e':
				// num_xstreams = atoi(optarg);
				// cout << num_xstreams << " are needed." << endl;
				break;
			case 'n':
				n = atoi(optarg);
				break;
			case 'h':
			default:
				printf("Usage: ./stdThread [-e NUM_XSTREAMS]\n");
				return -1;
		}
	}


	/* Used to Initialize thread_Singleton */
	#ifdef XTHREAD 
	fibonacci_arg_t temp = {3, 0};
	fibonacci_xthread (&temp);
	#endif
	#ifdef XTHREAD_D 
	fibonacci_arg_t temp = {3, 0};
	fibonacci_xthread_d (&temp);
	#endif



	/* Used to Initialize raw ABT_thread */
	#ifdef RAW_ABT_THREAD
	// Necessary variables  
	int i, j;
	int num_xstreams = atoi(getenv("NUM_ES"));
	// Allocate memory
	ABT_xstream *xstreams =
        (ABT_xstream *)malloc(sizeof(ABT_xstream) * num_xstreams);
    pools = (ABT_pool *)malloc(sizeof(ABT_pool) * num_xstreams);
    ABT_sched *scheds = (ABT_sched *)malloc(sizeof(ABT_sched) * num_xstreams);
    // Initialize Argobots
    ABT_init(argc, argv);
    // Create pools.
    for (i = 0; i < num_xstreams; i++) {
        ABT_pool_create_basic(ABT_POOL_FIFO, ABT_POOL_ACCESS_MPMC, ABT_TRUE,
                              &pools[i]);
    }
    // Create schedulers
    for (i = 0; i < num_xstreams; i++) {
        ABT_pool *tmp = (ABT_pool *)malloc(sizeof(ABT_pool) * num_xstreams);
        for (j = 0; j < num_xstreams; j++) {
            tmp[j] = pools[(i + j) % num_xstreams];
        }
        ABT_sched_create_basic(ABT_SCHED_DEFAULT, num_xstreams, tmp,
                               ABT_SCHED_CONFIG_NULL, &scheds[i]);
        free(tmp);
    }
    // Set up a primary execution stream
    ABT_xstream_self(&xstreams[0]);
    ABT_xstream_set_main_sched(xstreams[0], scheds[0]);
    // Create secondary execution streams
    for (i = 1; i < num_xstreams; i++) {
        ABT_xstream_create(scheds[i], &xstreams[i]);
	}
	#endif


	fibonacci_arg_t arg;
	chrono::steady_clock::time_point start = chrono::steady_clock::now();

	// for (int i = 0; i < 100; i++) 
	// {
		arg = {n, 0};
		/* raw ABT_thread */
		#ifdef RAW_ABT_THREAD
		fibonacci_raw(&arg);
		#endif

		/* For normal thread */
		#ifdef THREAD
		fibonacci_thread(&arg);
		#endif

		/* For normal xthread */
		#ifdef XTHREAD
		fibonacci_xthread(&arg);
		#endif

		/* For anounymous xthread */
		#ifdef XTHREAD_D
		fibonacci_xthread_d(&arg);
		#endif
	// }

	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double> >(end-start);
	cout << "Execution time: " << time_span.count() << endl;
	cout << "Execution time: " << (double)(time_span.count() / 100) << endl;
	int ret = arg.ret;
    int ans = fibonacci_seq(n);
	cout << "The returned value is " << ret << "; The verification is " << ans << endl;

	/* Used to Free the Allocated Resource of Raw ABT_thread */
	#ifdef RAW_ABT_THREAD
    // Join secondary execution streams
    for (i = 1; i < num_xstreams; i++) {
        ABT_xstream_join(xstreams[i]);
        ABT_xstream_free(&xstreams[i]);
    }
    // Finalize Argobots
    ABT_finalize();
    // Free allocated memory
    free(xstreams);
    free(pools);
    free(scheds);
	#endif

	/* TEST BLOCK */
	// start = chrono::steady_clock::now();
    //
	// fibonacci_arg_t arg_d = {n, 0};
    // fibonacci(&arg_d);
    // int ret_d = arg_d.ret;
    // int ans_d = fibonacci_seq(n);
	// cout << "The returned value is " << ret_d << "; The verification is " << ans_d << endl;

	
	// end = chrono::steady_clock::now();
	// time_span = chrono::duration_cast<chrono::duration<double> >(end-start);
	// cout << "Execution time: " << time_span.count() << endl;

	return 1;
}

