#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using std::cout;
using std::endl;

/**
 * Amount of iterations
 */
const unsigned int N = 1000;

/**
 * Number of threads ordered in a ring
 * 2 for even odd seperation
 * Higher values form a ring (eg: RING_SIZE = 5, Thread 3 prints values "2" "5" "8" "11" etc)
 */
const unsigned int RING_SIZE = 2;

unsigned int ringIndex = 0;
unsigned int counter = 0;
std::mutex ringMutex;
std::condition_variable cv;

void threadCounter(unsigned int offset)
{
	while (counter < N)
	{
		std::unique_lock<std::mutex> lk(ringMutex);
		cv.wait(lk,
				[=, &offset]()
				{
					// cout << "[T" << offset << "] CV notified, index is " << ringIndex << "" << endl;
					return ringIndex == offset || counter >= N;
				});

		if (counter >= N)
		{
			lk.unlock();
			cv.notify_all();
			return;
		}

		if (ringIndex >= RING_SIZE - 1)
		{
			ringIndex = 0;
		}
		else
		{
			++ringIndex;
		}

		cout << "[Thread " << offset << "] Counter: " << counter << endl;
		++counter;

		lk.unlock();
		cv.notify_all();
	}
}

int main()
{
	std::vector<std::thread> threads;
	for (size_t i = 0; i < RING_SIZE; ++i)
	{
		threads.emplace_back(threadCounter, (unsigned int)i);
	}

	cv.notify_all();

	for (auto &i : threads)
	{
		i.join();
	}
}