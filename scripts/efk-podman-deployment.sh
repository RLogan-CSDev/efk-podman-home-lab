#!/usr/bin/env bash
set -e		# Forces script to halt immediately if error occurs

echo "--- [1/4] Configuring Virtual Memory in Kernel --- "
sudo sysctl -w vm.max_map_count=262144
# Applies the memory map limit immediately for the active session
echo "vm.max_map_count=262144" | sudo tee /etc/sysctl.d/99-elasticsearch.conf
# Makes the setting persistent across system reboots

echo "--- Confirming [1/4] ---"
cat /proc/sys/vm/max_map_count
echo "Expected Output = 262144"

echo "--- [2/4] Creating BTRFS Partition & Target Directories ---"
if [ ! -d "/mnt/efk_stack" ]; then
  sudo btrfs subvolume create /mnt/efk_stack
# Creates the dedicated BTRFS partition
fi
sudo btrfs quota enable /mnt/efk_stack
# Enables the quota subsystem globally on the filesystem path
sudo btrfs qgroup limit 100g /mnt/efk_stack
# Enforces a strict limit of 100 GB

sudo chown -R "$USER:$USER" /mnt/efk_stack
# Transfers ownership to local user, allowing for necessary clearance
# for creating and managing log files on disk
mkdir -p /mnt/efk_stack/{elasticsearch,kibana,filebeat}
# Creates dedicated directories for each element in EFK Stack
mkdir -p "$HOME/logs"
# Creates dedicated directory outside of pod for logs

echo "--- Confirming [2/4] ---"
sudo btrfs qgroup show -pcre /mnt/efk_stack
echo "Expected Output = table showing 100GB max"

echo "--- [3/4] Initializing Podman Pod ---"
if ! podman pod exists efk-pod; then
  podman pod create --name efk-pod -p 5601:5601
# Limits pod external communication to port 5601, allowing access to Kibana's
# web dashboard.
fi

podman unshare chown -R 1000:1000 /mnt/efk_stack/elasticsearch
podman unshare chown -R 1000:1000 /mnt/efk_stack/kibana
podman unshare chown -R 1000:1000 /mnt/efk_stack/filebeat
# Maps the external elasticsearch UID/GID to the pod's internal namespace

echo "--- Confirming [3/4] ---"
podman pod ls
echo "Expected Output = efk-pod running with port 5601 active"

echo "--- [4/4] Deploying the EFK Stack ---"
# 1. Elasticsearch 9.4.2
# The -v option binds to the BTRFS partion created
# The :Z flag allows the container to write to disk via SELinux
if ! podman container exists elasticsearch; then
  podman run -d --pod efk-pod --name elasticsearch \
  -e "discovery.type=single-node" \
  -e "xpack.security.enabled=false" \
  -e "xpack.security.http.ssl.enabled=false" \
  -e "ES_JAVA_OPTS=-Xms512m -Xmx512m" \
  -v /mnt/efk_stack/elasticsearch:/usr/share/elasticsearch/data:Z \
  docker.elastic.co/elasticsearch/elasticsearch:9.4.2
fi

# 2. Kibana 9.4.2
# The -e option allows for internal querying from Elasticsearch on port 9200
if ! podman container exists kibana; then
  podman run -d --pod efk-pod --name kibana \
  -e "ELASTICSEARCH_HOSTS=http://localhost:9200" \
  docker.elastic.co/kibana/kibana:9.4.2
fi

# 3. Filebeat 9.4.2
# The ro flag gives read-only access to Filebeat on Podman's default log
# log directory and the log directory created under $HOME
if ! podman container exists filebeat; then
  podman run -d --pod efk-pod --name filebeat \
  -v ~/.local/share/containers/storage/overlay-containers:/var/log/containers:ro,z \
  -v "$HOME/logs:/var/log/host_apps:ro,z" \
  docker.elastic.co/beats/filebeat:9.4.2
fi
echo "-- Confirming [4/4] ---"
podman ps --pod
echo "Expected Output = 3 containers under efk-pod saying Up"

echo "--- Deployment Complete ---"
echo "Open http://localhost:5601 in your browser to access Kibana."
