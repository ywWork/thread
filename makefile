all:
	g++-10 --std=c++17 -I /Users/mt1ger/argobots-install/include/ -labt -L /Users/mt1ger/argobots-install/lib/ fib.cpp thread.cpp thread_Singleton.cpp -o test
