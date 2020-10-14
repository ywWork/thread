#include "thread.h"

argobots::thread::thread(void (* func)(void*), void *argu)
{
	int rank;
	int flag;

	psingleton = Singleton::instance();

	ABT_xstream_self_rank(&rank);

	ABT_pool target_pool = psingleton->pools[rank]; 

	flag = ABT_thread_create(target_pool, func, argu,
			ABT_THREAD_ATTR_NULL, &ult);
	tid = psingleton->Gtid;
	psingleton->Gtid++;
}

argobots::thread::thread (thread&& other) 
{
	swap (other);
}


void argobots::thread::join ()
{
	ABT_thread_free (&ult);
	psingleton->Gtid--;
}

ABT_thread_id argobots::thread::get_id () const noexcept
{
	return tid;
}

void argobots::thread::swap(thread & other) 
{
	std::swap (this->tid, other.tid);
}

argobots::thread& argobots::thread::operator=(thread&& other)
{
	
	this->ult = other.ult;
	this->tid = other.tid;
	return *this;
}



