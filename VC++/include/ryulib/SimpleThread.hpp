#ifndef RYULIB_SIMPLETHREAD_HPP
#define RYULIB_SIMPLETHREAD_HPP

#include <windows.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;

class SimpleThread;

typedef function<void(SimpleThread*)> SimpleThreadEvent;

class SimpleThread
{
private:
	std::thread thread_;
	std::mutex mutex_;
	std::condition_variable_any condition_;
private:
	bool is_terminated_;
public:
	SimpleThread(SimpleThreadEvent execute)
	{
		is_terminated_ = false;

		thread_ = std::thread(
			[&]()
			{
				execute(this);
			}
		);
	}
		
	~SimpleThread()
	{
		thread_.detach();
	}

	void Sleep(int millis)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait_for(lock, std::chrono::milliseconds(millis));
	}

	void SleepTight()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait(lock);
	}

	void WakeUp()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.notify_all();
	}

	void Terminate()
	{
		is_terminated_ = true;
		WakeUp();
	}

	void TerminateAndWait()
	{
		is_terminated_ = true;
		WakeUp();
		thread_.join();
	}

	void TerminateNow()
	{
		is_terminated_ = true;
		TerminateThread(thread_.native_handle(), 0);
	}
public:
	bool isTerminated() { return is_terminated_;  }
};

#endif  // RYULIB_SIMPLETHREAD_HPP