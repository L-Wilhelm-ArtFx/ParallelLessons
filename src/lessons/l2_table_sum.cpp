#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

using std::cout;
using std::endl;

const unsigned int N = 1000000; // count of values
const unsigned int BUCKET_SIZE = 10000;

std::vector<int> values;

int main()
{

	// Populate vector
	values.reserve(N);
	for (size_t i = 0; i < N; ++i)
	{
		values.push_back(i);
	}

	// 1 - Sequential
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		long long sum = 0.0;
		for (auto it = values.begin(); it != values.end(); ++it)
		{
			sum += *it;
		}

		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDur = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart);

		cout << "(1) Sum: " << sum << " Time: " << tDur << endl;
	}

	// 2 - Parallel buckets
	//     The vector is divided into (BUCKET_SIZE) chunks and a thread is launched for each.
	//     Finally the main thread sums up the resulting sub-sums.
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		std::vector<std::thread> threadPool;
		std::vector<long long> bucketSums;
		bucketSums.resize(N / BUCKET_SIZE + (N % BUCKET_SIZE != 0));

		size_t threadIx = 0;
		for (size_t i = 0; i < N; i += BUCKET_SIZE)
		{
			size_t bucketStart = i;
			size_t bucketEnd = i + BUCKET_SIZE;
			if (bucketEnd > N)
			{
				bucketEnd = N;
			}
			threadPool.emplace_back(
				[=, &bucketSums]()
				{
					long long sum = 0;
					for (size_t i = bucketStart; i < bucketEnd; ++i)
					{
						sum += values[i];
					}
					bucketSums[threadIx] = sum;
				});
			++threadIx;
		}

		for (auto it = threadPool.begin(); it != threadPool.end(); ++it)
		{
			it->join();
		}

		// Sum the buckets
		long long sum = 0;
		for (auto it = bucketSums.begin(); it != bucketSums.end(); ++it)
		{
			sum += *it;
		}

		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDur = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart);

		cout << "(2) Sum: " << sum << " Time: " << tDur << " Thread count: " << threadPool.size() << endl;
	}

	// (3) Parallel bucket distribution with mutexes
	//     Allocates a pool of threads according to hardware specs
	//     Threads take chunks (BUCKET_SIZE) at a time, synchronizing the index and sum inbetween iterations using mutexes.
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		long long sum = 0;
		size_t index = 0;
		std::mutex sumMutex;
		std::mutex indexMutex;

		// Concurrent threads supported
		unsigned int concCount = std::thread::hardware_concurrency();
		concCount = concCount > 0 ? concCount : 2; // Incase of detection failure default to 2 threads

		const size_t maxBuckets = N / BUCKET_SIZE + (N % BUCKET_SIZE != 0);

		std::vector<std::thread> threadPool;

		for (size_t i = 0; i < concCount && i < maxBuckets; ++i)
		{
			threadPool.emplace_back(
				[&]()
				{
					while (true)
					{
						size_t localIndex;
						size_t indexEnd;

						{
							std::lock_guard<std::mutex> indexGuard(indexMutex);
							localIndex = index;
							indexEnd = localIndex + BUCKET_SIZE;
							index = indexEnd;
						}

						if (localIndex >= N)
							break;

						if (indexEnd > N)
							indexEnd = N;

						long long localSum = 0;
						for (size_t i = localIndex; i < indexEnd; ++i)
						{
							localSum += values[i];
						}

						{
							std::lock_guard<std::mutex> sumGuard(sumMutex);
							sum += localSum;
						}
					}
				});
		}

		for (auto it = threadPool.begin(); it != threadPool.end(); ++it)
		{
			it->join();
		}

		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDur = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart);

		cout << "(3) Sum: " << sum << " Time: " << tDur << " Thread count: " << threadPool.size() << endl;
	}
}