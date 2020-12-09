#ifndef __THREAD_H
#define __THREAD_H

#include<abt.h>
#include<functional>
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<tuple>
#include"thread_Singleton.h"


namespace stdx 
{

	template<class Ret, class ...Args>
	struct xthread_wrapper_args_t 
	{
		std::function<Ret(Args...)> func_;
		std::tuple<Args...> tuple_;
		ABT_eventual* ev_ptr_;
		// Ret ret_;
	};


	// class thread_d 
	// {
    //
	// 	template<class Fn, class ...Args>
	// 	friend
	// 	void t_wrapper (void * ptr);
    //
	// 	public:
	// 		thread_Singleton* psingleton;
    //
	// 		#<{(| default constructor |)}>#
	// 		thread_d () noexcept {}
    //
	// 		#<{(| constructor with parameters |)}>#
	// 		template<class Fn, class ...Args>
	// 		thread_d (Fn func, Args ...args)
	// 		{
	// 			int rank;
	// 			int flag;
    //
	// 			wa_<Fn, Args...>.func = func;
	// 			wa_<Fn, Args...>.tuple_ = std::make_tuple(args...);
	// 			ABT_eventual_create(0, &eventual);
	// 			wa_<Fn, Args...>.ev_ptr = &eventual;
    //
	// 			#<{(| Initializing pools, schedulors and ESs in singleton class |)}>#
	// 			#<{(| And offer a handler to reach the resources for this ULT |)}>#
	// 			psingleton = thread_Singleton::instance();
    //
	// 			ABT_xstream_self_rank(&rank);
	// 			ABT_pool target_pool = psingleton->pools[rank]; 
	// 			flag = ABT_thread_create(target_pool, t_wrapper<Fn, Args...>, &wa_<Fn, Args...>,
	// 					ABT_THREAD_ATTR_NULL, nullptr);
	// 			tid = psingleton->Gtid;
	// 			psingleton->Gtid++;
	// 		}
    //
	// 		void wait();
    //
	// 	private:
	// 		template<class Fn, class ...Args>
	// 		static wrapper_args_t<Fn, Args...> wa_;
    //
	// 		ABT_eventual eventual;
	// 		ABT_thread_id tid;
	// };

	template<class Ret, class ...Args> 
	inline void xthread_wrapper (void * ptr) 
	{
		stdx::xthread_wrapper_args_t<Ret, Args...>* xwa_ptr = (stdx::xthread_wrapper_args_t<Ret, Args...>*) ptr;
		std::apply(xwa_ptr->func_, xwa_ptr->tuple_);
	}

	class thread
	{
		public:
			class id 
			{
				ABT_thread ult;
				public:
				id () noexcept : ult (){}
				id (ABT_thread other): ult(other) {}

				private:
					friend class thread;
					friend bool operator== (thread::id id1, thread::id id2) noexcept;
					friend bool operator< (thread::id id1, thread::id id2) noexcept;

					friend 
					ostream&
					operator<< (ostream& __out, thread::id id1);
			};

		public:
			thread_Singleton* psingleton;

			/* default constructor */
			thread () noexcept {}

			/* constructor with parameters */
			template<class Fn, class ...Args>
			thread (Fn func_in, Args ...args)
			{
				int rank;
				int flag;
			
				psingleton = thread_Singleton::instance();
				typedef typename std::result_of<decltype(func_in)&(Args...)>::type mytype_; 

				wrapper_args_<mytype_, Args...>.func_ = func_in;
				wrapper_args_<mytype_, Args...>.tuple_ = std::make_tuple(args...);

				std::apply(wrapper_args_<mytype_, Args...>.func_, wrapper_args_<mytype_, Args...>.tuple_);
				// ABT_xstream_self_rank(&rank);
				// ABT_pool target_pool = psingleton->pools[rank]; 
				// // flag = ABT_thread_create(target_pool, xthread_wrapper<mytype_, Args...>, nullptr,
				// flag = ABT_thread_create(target_pool, xthread_wrapper<mytype_, Args...>, &wrapper_args_<mytype_, Args...>,
				// 		ABT_THREAD_ATTR_NULL, &__id.ult);
				psingleton->Gtid++;
			}

			thread (thread&& other);
			thread (thread&) = delete;
			thread (const thread&) = delete;
			thread (const thread&&) = delete;

			~thread() {}

			void join ();
			void detach();
			bool joinable ();
			id get_id () const noexcept;
			void swap(thread & other);

			thread& operator=(thread&& other);

		private:
			id __id;

			template<class Ret, class ...Args>
			static inline xthread_wrapper_args_t<Ret, Args...> wrapper_args_ = {};
			// template<class Ret, class ...Args> 
			// inline void xthread_wrapper (void * ptr) 
			// {
			// 	
			// 	std::apply(wrapper_args_<Ret, Args...>.func_, wrapper_args_<Ret, Args...>.tuple_);
			// }
	};

	ostream& operator<<(ostream& __out, thread::id id2);
	bool operator==(thread::id id1, thread::id id2) noexcept;
	bool operator<(thread::id id1, thread::id id2) noexcept;
	bool operator>(thread::id id1, thread::id id2) noexcept;
	bool operator!=(thread::id id1, thread::id id2) noexcept;
	bool operator>=(thread::id id1, thread::id id2) noexcept;
	bool operator<=(thread::id id1, thread::id id2) noexcept;
};

#endif


