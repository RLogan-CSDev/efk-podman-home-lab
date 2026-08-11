#ifndef BENCHMARK_LOGGER_HPP
#define BENCHMARK_LOGGER_HPP

#include <fstream>
#include <iostream>
#include <string>

class BenchmarkLogger{
private:
    std::string logFilePath;

public:
    BenchmarkLogger(const std::string& filePath ="logs/benchmarks.log"){
        logFilePath = filePath;
    }

    void log(const std::string& funcName, int inputSize, double durationInMS){
        std::ofstream file(logFilePath, std::ios::app);
        if(file.is_open()){
            file << "{\"function_name\":\"" << funcName
                 << "\",\"input_size\":" << inputSize
                 << ",\"duration_ms\":" << durationInMS << "}\n";
        }
        else{
            std::cerr << "[BenchmarkLogger] Error: Unable to open " << logFilePath << "\n";
        }
    }
};
#endif /* BENCHMARK_LOGGER_HPP */