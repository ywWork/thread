all:
	g++-10 -I /Users/mt1ger/argobots-install/include/ -labt -L /Users/mt1ger/argobots-install/lib/ stdThread.cpp -o test
