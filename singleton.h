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

	Singleton(int num_xstreams_in, int argc, char** argv)
	{
		Gtid = 0;
		num_xstreams = num_xstreams_in;
	    // ABT_init(argc, argv);
		mem_allocation (num_xstreams);
		cout << "after mem_allocation" << endl;
		pools_scheds_creation ();
		cout << "after pools_scheds_creation"<< endl;
		main_xstream ();
		cout << "after main_xstream"<< endl;
		secondary_xstreams (num_xstreams);
		cout << "after secondary_xstreams"<< endl;
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
	static Singleton* instance (int num_xstreams_in, int argc, char** argv);
	ABT_pool* pools; 
};
