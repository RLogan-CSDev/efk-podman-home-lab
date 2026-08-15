#ifndef BENCHMARK_TIMER_HPP
#define BENCHMARK_TIMER_HPP

/*
* See cplusplus.com/reference/chrono and/or geeksforgeeks.org/cpp/chrono-in-c
* for more information on the implementation details done here.
*/

#include "BenchmarkLogger.hpp"
#include <chrono>

class BenchmarkTimer{
private:
    chrono::time_point<chrono::steady_clock> startTime;
    chrono::time_point<chrono::steady_clock> endTime;
    bool isRunning;

    string taskName;
    int inputSize;
    BenchmarkLogger& logger;

public:
    BenchmarkTimer(const string& task, int input, BenchmarkLogger& log) 
        : taskName(task), inputSize(input), logger(log) {
            // Parameterized constructor that uses list initializer to assign class variables
            startBenchmark();
    }

    ~BenchmarkTimer(){
        // Triggers stop when object goes out of scope in case not stopped using stopBenchmark()
        if(isRunning) {
            stopBenchmark();
        }
    }

    void startBenchmark(){
        startTime = chrono::steady_clock::now();
        isRunning = true;
    }

    double getDuration() const {
        chrono::time_point<chrono::steady_clock> currentTime;
        if (isRunning) {
            currentTime = chrono::steady_clock::now();
        }
        else {
            currentTime = endTime;
        }
        
        auto elapsedTime = duration_cast<chrono::microseconds>(currentTime - startTime);

        return elapsedTime.count();
    }

    double stopBenchmark() {
        // Using return type double gives flexibility for output to both 
        // log file AND console, if needed.
        if (!isRunning) {
            return getDuration();
        }
        else {
            endTime = chrono::steady_clock::now();
            isRunning = false;
            double duration = getDuration();
            logger.log(taskName, inputSize, duration);
            return duration;
        }
    }
};

#endif /* BENCHMARK_TIMER_HPP */