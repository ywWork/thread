#include<vector>
#include <iostream>
#include <stdlib.h>

#include <thread>
#include <ctime>

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdarg.h>
#include <abt.h>
// #include "abt.h"
// #include "/Users/mt1ger/argobots-install/include/abt.h"

#define DEFAULT_NUM_XSTREAMS 2

using namespace std;

/* Global Variable */
ABT_xstream* xstreams;
ABT_pool* pools;
ABT_sched* scheds;

ABT_thread * Gthreads;

long long tid_glob;
int cnter=0;

typedef struct {
    int n;
    int ret;
} fibonacci_arg_t;


typedef struct  
{
	// ABT_xstream * inClass_xstreams;
	// ABT_sched* inClass_scheds;
	ABT_pool * inClass_pools;
	int tid;
	int aaa;
	int bbb;
} arguments;


void DoNothing()
{

}

void blahblah (int tid)
{
	printf("the tid is %d", tid);
}

namespace argobots 
{
	void mem_allocation (int num_xstreams) 
	{
		xstreams = (ABT_xstream *)malloc(sizeof(ABT_xstream) * num_xstreams);
		pools = (ABT_pool *)malloc(sizeof(ABT_pool) * num_xstreams);
		scheds = (ABT_sched *)malloc(sizeof(ABT_sched) * num_xstreams);
		Gthreads = (ABT_thread *)malloc(sizeof(ABT_thread) * 100);
	}

	void pools_scheds_creation (int num_xstreams) 
	{
		/* Create pools. */
		for (int i = 0; i < num_xstreams; i++) {
			ABT_pool_create_basic(ABT_POOL_FIFO, ABT_POOL_ACCESS_MPMC, ABT_TRUE,
					&pools[i]);
		}

		/* Create schedulers. */
		for (int i = 0; i < num_xstreams; i++) {
			ABT_pool *tmp = (ABT_pool *)malloc(sizeof(ABT_pool) * num_xstreams);
			for (int j = 0; j < num_xstreams; j++) {
				tmp[j] = pools[(i + j) % num_xstreams];
			}
			ABT_sched_create_basic(ABT_SCHED_DEFAULT, num_xstreams, tmp,
					ABT_SCHED_CONFIG_NULL, &scheds[i]);
			free(tmp);
		}
	}

	void main_xstream ()
	{
		ABT_xstream_self(&xstreams[0]);
		ABT_xstream_set_main_sched(xstreams[0], scheds[0]);
	}

	void secondary_xstreams (int num_xstreams) 
	{
		for (int i = 1; i < num_xstreams; i++)
		{
			ABT_xstream_create(scheds[i], &xstreams[i]);
		}
	}

	void finalize()
	{
		/* Finalize Argobots. */
		ABT_finalize();

		/* Free allocated memory. */
		free(xstreams);
		free(pools);
		free(scheds);
	}





	class thread
	{
		public:
			long long tid;
			int test;
			ABT_thread ult;

			/* default constructor */
			thread () 
			{

			}
			template <class funcType, class arguType>
			thread(funcType func, arguType argu)
			// thread(void (* func)(void*), void *argu)
			{
				int rank;
				int flag;

				ABT_xstream_self_rank(&rank);
				ABT_pool target_pool = pools[rank];
				flag = ABT_thread_create(target_pool, func, argu,
						// ABT_THREAD_ATTR_NULL, &Gthreads[cnter]);
						ABT_THREAD_ATTR_NULL, &ult);
				if (flag == 0)
					cnter++;
				tid = tid_glob;
				// printf("Thread %lld created, %d\n", tid_glob, flag);
				tid_glob++;
			}
			~thread()
			{
				
			}

			void join () 
			{
				ABT_thread_free (&ult);
				tid_glob--;
			}

			long long get_id() 
			{
				return tid;
			}

			/* NOT finished */
			thread (thread& other) 
			{
				this->ult = other.ult;
			}


			thread& operator=(thread&& other)
			{
				
				this->ult = other.ult;
				this->tid = other.tid;
				return *this;
			}

			// thread& swap (thread& other) 
			// {
			// 	threadSwap((*this), other);
			// }

		private:
	};

};


void fibonacci(void *arg)
{
    int n = ((fibonacci_arg_t *)arg)->n;
    int *p_ret = &((fibonacci_arg_t *)arg)->ret;

    if (n <= 1) {
        *p_ret = 1;
    } 
	else {
        fibonacci_arg_t child1_arg = {n - 1, 0};
        fibonacci_arg_t child2_arg = {n - 2, 0};

		argobots::thread threads (fibonacci, &child1_arg);

        /* Calculate fib(n - 2).  We do not create another ULT. */
        fibonacci(&child2_arg);

		threads.join();
		// ABT_thread_free(&(threads.ult));

        *p_ret = child1_arg.ret + child2_arg.ret;
    }
}

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

void blah (void *argu) 
{
	int n = ((fibonacci_arg_t*)argu)->n;
	int ret = ((fibonacci_arg_t*)argu)->ret;
	
	printf("n is %d, ret is %d\n", n, ret);
	
	((fibonacci_arg_t*)argu)->ret = n*n; 
	printf("ret is %d\n", ((fibonacci_arg_t*)argu)->ret);
}

void blah (int * argu) 
{

	printf("argu is %d\n", *argu);

}

int main (int argc, char * argv[])
{

	int num_xstreams = DEFAULT_NUM_XSTREAMS;
	int n = 13;

	/* Read arguments. */
	while (1) {
		int opt = getopt(argc, argv, "he:n:");
		if (opt == -1)
			break;
		switch (opt) {
			case 'e':
				num_xstreams = atoi(optarg);
				cout << num_xstreams << " are needed." << endl;
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


	/* *Argobots Preparation* */
    /* Memory Allocation */ 
	argobots::mem_allocation (num_xstreams);
	// cout << "mem_allocation successes." << endl;
    /* Initialize Argobots */
    ABT_init(argc, argv);
	// cout << "ABT_init successes." << endl;
	/* Create Pools and Schedulers */
	argobots::pools_scheds_creation (num_xstreams); 
	// cout << "pools and schedulers are created successfully." << endl;
    /* Set up a primary execution stream */
	argobots::main_xstream ();
	// cout << "main xstreams and schedulers are created successfully." << endl;
	/* Set up secondary execution stream */
	argobots::secondary_xstreams (num_xstreams);
	// cout << "xstreams and schedulers are created successfully." << endl;


    
	fibonacci_arg_t arg = {n, 0};
    fibonacci(&arg);
	cout << "The counter is " << cnter << endl;
	cout << "Finished fib computation." << endl;
    int ret = arg.ret;
    int ans = fibonacci_seq(n);
	cout << "The ret is " << ret << " the ans is " << ans << endl;

	argobots::thread * threadArr;
	threadArr = (argobots::thread*)malloc(n*sizeof(argobots::thread));
	fibonacci_arg_t * args;
	args = (fibonacci_arg_t*)malloc(n * sizeof (fibonacci_arg_t));
	//
	// argobots::thread threadArr[n];
	// for (int i = 0; i < n; i++)
	// {
	// 	// args[i] = {i, 0};
	// 	fibonacci_arg_t args = {i, 0}; 
	// 	threadArr[i] = argobots::thread(blah, &args);
	// 	threadArr[i].join();
	// }

    for (int i = 1; i < num_xstreams; i++) 
	{
        ABT_xstream_join(xstreams[i]);
        ABT_xstream_free(&xstreams[i]);
    }
	
	free(xstreams);	
	free(pools);	
	free(scheds);	
	ABT_finalize();
	return 1;
}

