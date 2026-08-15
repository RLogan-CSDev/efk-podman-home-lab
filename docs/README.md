# C++ Algorithmic Performance Benchmarking via Containerized, Rootless EFK Observability Stack
Developer: Robert Logan

## Project Overview
An automated deployment of an Elasticsearch, Filebeat, and Kibana (EFK) observability stack on Fedora Workstation with a custom C++ application that generates structured JSON telemetry in an end-to-end performance benchmark.

### Objective
*Creating an environment that allows profiling in C++ applications for analysis in Big-O runtime performance telemetry. Testing the environment with a set of sorting algorithms already designed.*

---

## Tech Stack
* Fedora Workstation - Linux
* Podman (rootless pods)
* C++20 (std::chrono, std::algorithm)
* Elastic Stack 9.4.2 (Elasticsearch, Filebeat, Kibana)
* Visual Studio Code
* Bash scripting

---

## Lab Structure
* `benchmark/` - contains C++ classes, driver file, and logs generated
* `config` - contains a copy of the filebeat.yml file (original stored on subvolume holding EFK stack)
* `docs` - contains pertinent documentation
* `scripts` - contains idempotent script for automation of EFK stack setup

---

## Key Engineering Highlights
* **System Kernel Tuning**: Virtual memory mappings had to be adjusted so that Elasticsearch can run properly with crashing.
* **Disk Partitioning**: Using the BTRFS partitioning method native to Fedora, risk shifts from host root filesystem during log ingestion.
* **Container Orchestration**: Using Fedora's native Podman package allows for network segmentation using the created pod's internal communication network.
* **Idempotent Automation**: Creating a Bash script that not only maintains a history of commands used for implementation, but also a way to re-run the script so that when updates are added, the installations/configurations already performed do not attempt to perform again.
* **Custom C++ Application**: Modularized C++ code that uses the `std::chrono` library to create a benchmark necessary for Big-O runtime performance analysis by generating output into JSON format.
* **Log Parsing**: Configuring Filebeat to pull data from the log file generated in the C++ application into the Elasticsearch database.

---

## System/Observability Pipeline
1. `C++ Application` --> `benchmarks.log` --> `Filebeat ingestion` --> `Elasticsearch storage` --> `Kibana Lens`

2. ***efk-pod*** [ `Filebeat container` + `Elasticsearch container` + `Kibana container` ]

3. ***efk_stack*** [ `elasticsearch files` + `filebeat files` + `kibana files` ]

---

## Reproduction Steps

#### How to create the `efk-pod` and start the stack:
1. Change the memory map limit across system reboots
2. Partition the hard drive using BTRFS method
3. Initialize pod only on port 5601
4. Deploy EFK stack
***Check the script*** `efk-podman-deployment.sh` ***for more details about command usage.***

#### How to compile and run C++ benchmark binary:
1. Pull all .hpp and .cpp files from repo
2. Ensure there is a `logs/` directory created with a `benchmarks.log` file inside
3. Use the command to generate executable: `g++ -std=c++20 -I include src/main.cpp -o benchmark_experiment`
**NOTE**: The above command assumes the following
            - The compiler uses C++ version 20
            - The file system structure mirrors this repo (`-I` option)
            - The location of command execution is inside the `benchmark` directory
4. Use the command to run executable: `./benchmark_experiment`
5. Verify logs generated: `cat logs/benchmarks.log`

#### How to load Kibana Lens:
1. Pull `filebeat.yml` configuration file. The main file is kept inside the partition with the rest of EFK stack files. Although, for replication, the file was copied into the `config` directory.
2. Edit the `paths` option to where the aforementioned C++ `logs/` directory was created.
3. Edit the `efk-podman-deployment.sh` script under the **Filebeat** installation section. Change the `-v` mount options to satisfy the following format
            - /path/to/config/filebeat.yml:/KEEP/SAME
            - /path/to/logs:/KEEP/SAME
4. Re-run the `efk-podman-deployment.sh` script. The `--replace` option will maintain container name while changing the mounting options.
5. Use the command to start the pod: `podman pod start efk-pod`
6. Verify that Elasticsearch has the telemetry using the command: `podman run --rm -it --pod efk-pod docker.elastic.co/elasticsearch/elasticsearch:9.4.2 curl -X GET "localhost:9200/_cat/indices?v"`
**NOTE**: Since the host OS and pod have segmented networks, the only way Elasticsearch database can be retrieved is through the internal command execution inside the pod. The host OS can only communicate with the pod on port 5601. The verification is on port 9200, which is the port that Elasticsearch communicates on.

> ##### Creating the Kibana Data View
> 
> * Visit `http://localhost:5601` to access Kibana in a web browser.
> * On the left sidebar, click **Management** > **Stack Management**.
> * In the *Kibana* section, click **Data Views**.
> * Click **Create Data View**.
> * Add a name, such as *cpp-benchmark*
> * Add the index pattern "cpp-*"
> **NOTE**: If there isn't anything in the right-side suggesting the index pattern can match, then repeat the above steps.
> * Click *Save data view to Kibana*.
>
> ##### Building the Big-O Complexity Graph
> * On the left sidebar, click **Analytics** > **Visualize Libary**.
> * Click **Create Visualization** > **Lens**
> * In top-left dropdown, select the name of the data view created before.
> * Pull the *input_size* field to the horizontal axis.
> * Pull the *duration_us* field to the vertical axis.
> * Pull the *function_name.keyword* field to the Breakdown.
> * Edit the style of the graph as desired, such as setting the *Line interpolation* setting to *Smooth*.

## Results & Analysis

The algorithms used are:
[ **Bubble Sort**, **Insertion Sort**, **Quick Sort**, **std::sort** ]

The independent variable is the input size N. The dependent variable is the average of the runtime measured in milliseconds. 
The input size pool consisted of [ N = {1,000 | 2,500 | 5,000 | 10,000 | 25,000 | 50,000} ].
Each input size for each algorithm had 20 trials ran to data accuracy.

**Bubble Sort** & **Insertion Sort** produce O(N^2) complexity. **Quick Sort** & **std::sort** product O(Nlog(N)) complexity. 

After the C++ application ran, the Kibana Lens line graphs were produced.

![Kibana Lens Visual](images/sorting-algorithms-visuals.png)

* The linear scale on the y-axis shows the same relationships in the theoretical complexity trajectories in asymptotic analysis. For example, the only difference is that the **Bubble Sort** algorithm has a slower performance than the **Insertion Sort** by a constant factor.

* The logarithmic scale on the y-axis shows differences in the growth rates by orders of magnitude. This means that the quadratic algorithms are slower by multiplicative factors of 10 compared to the linearithmic algorithms.

* **Linear Scale** answers the question: *"How much real-world time does an operation cost?"*
* **Logarithmic Scale** answers the question: *"How efficient is the algorithm's design?"*

***Overall Assessment***
Unless the sorting algorithm has been optimized, the fastest way to sort data in this study is presented as the standard C++ library `algorithm` via the function `std::sort()`