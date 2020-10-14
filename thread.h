#ifndef __THREAD_H
#define __THREAD_H

#include<abt.h>
#include<cstdlib>

#include "singleton.h"


namespace argobots 
{

	class thread
	{
		public:
			ABT_thread ult;

			Singleton* psingleton;

			/* default constructor */
			thread () noexcept {}
			/* constructor with parameters */
			thread(void (* func)(void*), void *argu);
			thread (thread&& other);
			thread (thread&) = delete;
			thread (const thread&) = delete;
			thread (const thread&&) = delete;

			~thread() {}

			void join ();
			ABT_thread_id get_id () const noexcept;
			void swap(thread & other);

			thread& operator=(thread&& other);

		private:
			ABT_thread_id tid;

	};

};
#endif


