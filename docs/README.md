# Containerized EFK Observability Stack with Storage Isolation
An automated deployment of an Elasticsearch, Filebeat, and Kibana (EFK) observability stack on Fedora Workstation.

**Objective**: Creating an environment that allows profiling in C++ applications for analysis in Big-O runtime performance.

This project uses an idempotent Bash script (`efk-podman-deployment.sh`) to handle the following:
* **Tuning host OS kernel** virtual memory mapping max capacity to align with Elasticsearch requirements.
* **BTRFS hard drive partitioning** to shift risk from host root filesystem during log ingestion.
* **Container orchestration** via Fedora's native Podman package to allow network segmentation with the pod's internal communication network. *The intent with containerization is to have an on/off switch to preserve resources on a system.*

---

## System Architecture
```text
[ Host OS (Fedora Workstation) ]
    |
    |-BTRFS Partition (/mnt/efk_stack)
    |   |-Blast Radius Containment (protected root filesystem)
    |   |-Elasticsearch files
    |   |-Filebeat files
    |   |-Kibana files
    |-Rootless Podman Engine
        |-[ efk-pod ] (Internal Network Communication)
            |-Elasticsearch     (Port 9200 - Internal only)
            |-Filebeat          (Log Ingestion)
            |-Kibana            (Port 5601 - Open to Host)
```
---

## Implementation Notes
1. **Configuring the virtual memory in the kernel**:
Fedora restricts memory mapping to 65,530; however, Elasticsearch requires a minimum of 262,144 memory mappings. This is to prevent memory allocation crashes on boot, and since this only adjusts the virtual memory in the kernel, the physical RAM is not interfered with.
2. **Setting up the partition**:
Using the BTRFS partitioning method, sub-volumes are created to divide the big BTRFS pool that is currently keeping the entire drive. It is easier to do this since the physical volume has automatically been created already through the Fedora installation.
3. **Initializing Podman pod**:
The Podman container has its own internal shared network space that will allow only the EFK stack to talk with each other. Creating the container in this way allows for the practice of *Network Segmentation* so that the two distinct zones remain separated. These zones are the pod and the rest of the OS. In the pod zone, the internal network is isolated; therefore, allowing Filebeat and Kibana to communicate with Elasticsearch on `port 9200`. However, the rest of the OS cannot communicate on port 9200. The only port that the host OS is allowed to communicate with the EFK stack is through `port 5601`. This allows the host OS to access the Kibana web dashboard.
4. **EFK stack deployment**:
The EFK stack is pulled into the `efk-pod` container through their own separate "sub-containers" titled respectively to the function in the stack. The EFK stack configuration files are saved permanently on the sub-volume partition under `/mnt/efk_stack`. The container pod only enables active EFK data collection measures. Once the `efk-pod` is turned on, the data collection is pulled to the sub-volume. This is because the "sub-containers" are bind mounted to the partition.

---

## Operational Commands
* **Check Pod Status**: `podman pod ps`
* **Start/Stop Stack**: `podman pod start efk-pod` / `podman pod stop efk-pod`
* **Check Disk Usage**: `sudo btrfs qgroup show -r /mnt/efk_stack`
* **Confirm Kernel VM Limit**: `sysctl vm.max_map_count`