#ifndef THREAD_H
#define THREAD_H


#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <queue>
#include <functional>

//constexpr int NUM_THREADS = std::thread::hardware_concurrency() / 2;
//
//extern std::array<std::thread, NUM_THREADS> threads;

class TasksQueue {
public:
	void push(std::function<void()> task) {
		std::unique_lock<std::mutex> lock(mtx);
		tasks.push(task);
	}

	std::function<void()> pop() {
		std::unique_lock<std::mutex> lock(mtx);
		auto task = tasks.front();
		tasks.pop();
		return task;
	}

	bool empty() {
		std::unique_lock<std::mutex> lock(mtx);
		return tasks.empty();
	}
	//front function
	std::function<void()> front() {
		std::unique_lock<std::mutex> lock(mtx);
		return tasks.front();
	}
private:
	std::queue<std::function<void()>> tasks;
	std::mutex mtx;
};

class Threads {
public:
	Threads() {
		unsigned int numThreads = std::thread::hardware_concurrency();
		for (unsigned int i = 0; i < numThreads; ++i) {
			threads.push_back(std::thread([this]() {
				while (true) {
					std::unique_lock<std::mutex> lock(mtx);
					cv.wait(lock, [this]() { return !tasks.empty(); });
					auto task = tasks.front();
					tasks.pop();
					lock.unlock();
					task();
				}
			}));
		}
	}

	~Threads() {
		for (auto& thread : threads) {
			thread.join();
		}
	}

	void addTask(std::function<void()> task) {
		std::unique_lock<std::mutex> lock(mtx);
		tasks.push(task);
		cv.notify_one();
	}
private:
	std::vector<std::thread> threads;
	std::mutex mtx;
	std::condition_variable cv;
	TasksQueue tasks;

};

#endif