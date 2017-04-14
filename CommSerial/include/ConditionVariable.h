#pragma once
#include <condition_variable>
#include <chrono>
#include <algorithm>
#include <atomic>

// ConditionVariable Wrapper for CommProtocol.
class CommConditionVariable {
public:
	CommConditionVariable(bool ready = false)
		:ready(ready)
	{
	}

	/**
	Block the thread until Set
	*/
	void Wait() {
		std::unique_lock<std::mutex> lk(mu);
		cv.wait(lk, [&] {return ready; });
		ready = false;
	}
	/**
	Block the thread until Set or specified milliseconds pass
	*/
	void Wait(std::chrono::milliseconds waitMillis) {
		std::unique_lock<std::mutex> lk(mu);
		cv.wait_for(lk, waitMillis, [&] {return ready; });
		ready = false;
	}

	void Set() {
		std::lock_guard<std::mutex> lk(mu);
		ready = true;
		cv.notify_one();
	}
private:
	std::condition_variable cv;
	std::mutex mu;
	bool ready;
};