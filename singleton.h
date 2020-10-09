#include<cstdlib>
#include<iostream>
#include<abt.h>

using namespace std;

class Singleton 
{

	int num_xstreams = 0;	
	static Singleton * singleton_ptr;
	ABT_xstream * xstreams;
	ABT_sched* scheds; 
	ABT_thread_id Gtid;

	// int * aaa;

	Singleton(int num_xstreams_in)
	{
		Gtid = 0;
		num_xstreams = num_xstreams_in;
	    ABT_init(0, nullptr);
		mem_allocation (num_xstreams);
		pools_scheds_creation ();
		main_xstream ();
		secondary_xstreams (num_xstreams);
	}
	~Singleton() 
	{
		join_free_xstream (num_xstreams);
		finalize ();
	}


	void mem_allocation (int num_xstreams); 
	void pools_scheds_creation ();
	void main_xstream ();
	void secondary_xstreams (int num_xstreams);
	void join_free_xstream (int num_xstreams);
	void finalize ();

	public:
	static Singleton* instance (int num_xstreams_in);
	ABT_pool* pools; 
};
