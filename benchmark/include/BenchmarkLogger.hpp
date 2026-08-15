#ifndef BENCHMARK_LOGGER_HPP
#define BENCHMARK_LOGGER_HPP

/*
* See cplusplus.com/reference/fstream/ofstream/ofstream for more information on the
* implementation details seen here.
*/

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class BenchmarkLogger{
private:
    string logFilePath;

public:
    BenchmarkLogger(const string& filePath ="logs/benchmarks.log") {
        // Default constructor automatically assigning file path
        logFilePath = filePath;
    }

    void log(const string& funcName, int inputSize, double durationInMS) {
        ofstream file(logFilePath, ios::app);   // Appends output to file, standard = overwrite file
        if(file.is_open()) {
            /*
            * An example output for the file is as follows:
            * {"function_name":"quickSort","input_size":"10000","duration_ms":"1.164"}
            * {"function_name":"bubbleSort","input_size":"10000","duration_ms":"32.164"}
            * The file output is structured in JSON format so that Filebeat can parse the
            * fields without using complex regex conditions.
            */
            file << "{\"function_name\":\"" << funcName
                 << "\",\"input_size\":" << inputSize
                 << ",\"duration_ms\":" << durationInMS << "}\n";
        }
        else {
            cerr << "[BenchmarkLogger] Error: Unable to open " << logFilePath << "\n";
        }
    }
};
#endif /* BENCHMARK_LOGGER_HPP */