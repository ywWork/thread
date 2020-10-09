#include"singleton.h"

Singleton * Singleton::instance (int num_xstreams_in) 
{
    if (singleton_ptr == nullptr)
    {
        singleton_ptr = new Singleton(num_xstreams_in);
    }
    return singleton_ptr;
}

void Singleton::mem_allocation (int num_xstreams) 
{
	xstreams = (ABT_xstream *)malloc(sizeof(ABT_xstream) * num_xstreams);
	pools = (ABT_pool *)malloc(sizeof(ABT_pool) * num_xstreams);
	scheds = (ABT_sched *)malloc(sizeof(ABT_sched) * num_xstreams);
}

void Singleton::pools_scheds_creation () 
{
	/* Create pools. */
	for (int i = 0; i < num_xstreams; i++) {
		ABT_pool_create_basic(ABT_POOL_FIFO, ABT_POOL_ACCESS_MPMC, ABT_TRUE,
				&pools[i]);
	}


	/* Create schedulers */
	bool flag = 1;
	for (int i = 0; i < num_xstreams; i++) {
		ABT_pool *tmp = (ABT_pool *)malloc(sizeof(ABT_pool) * num_xstreams);
		for (int j = 0; j < num_xstreams; j++) {
			tmp[j] = pools[(i + j) % num_xstreams];
		}
		flag = ABT_sched_create_basic(ABT_SCHED_DEFAULT, num_xstreams, tmp,
				ABT_SCHED_CONFIG_NULL, &scheds[i]);
		free(tmp);
	}
}

void Singleton::main_xstream ()
{
	ABT_xstream_self(&xstreams[0]);
	ABT_xstream_set_main_sched(xstreams[0], scheds[0]);
}

void Singleton::secondary_xstreams (int num_xstreams) 
{
	for (int i = 1; i < num_xstreams; i++)
	{
		ABT_xstream_create(scheds[i], &xstreams[i]);
	}
}

void Singleton::join_free_xstream (int num_xstreams)
{
	for (int i = 0; i < num_xstreams; i++)
	{
		ABT_xstream_join (xstreams[i]);
		ABT_xstream_free (&xstreams[i]);
	}
}

void Singleton::finalize ()
{
	/* Finalize Argobots. */
	ABT_finalize();

	/* Free allocated memory. */
	free(xstreams);
	free(pools);
	free(scheds);
}

