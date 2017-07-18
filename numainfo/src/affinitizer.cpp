//
// Created by lingyan on 7/14/17.
//

#include "affinitizer.hpp"

// cpu_id = 0, 1, ... n-1, where n is the system's number of cores
void affinitize(int cpu_id) {
    //int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    unsigned int num_cores = std::thread::hardware_concurrency();
    if (cpu_id < 0 || cpu_id >= num_cores) {
        cerr << "affinitize(): invalid cpu id.\n";
        exit(-1);
    }

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    pthread_t current_thread = pthread_self();
    if (pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset) != 0) {
        cerr << "affinitize(): failed to affinitize, errno:" << std::strerror(errno) << endl;
    }
}

int getNumNUMAs() {
#ifdef DNUMA
    if (numa_available() == -1) {
        cerr << "getNUMACPUList(): numa is not available.\n";
        exit(-1);
    }

    int maxnode = numa_max_node();
    maxnode = maxnode + 1;
    return maxnode;
#else
    return -1;
#endif
}

int getNumNUMACPUs(int i) {
#ifdef DNUMA
    int ret = 0;
    if (i < 0 || i >= getNumNUMAs()) {
        cerr << "getNumNUMACPUs(): invalid numa id.\n";
        exit(-1);
    }

    unsigned int ncores = std::thread::hardware_concurrency();
    struct bitmask* cpu_list;
    cpu_list = numa_allocate_cpumask();
    if (numa_node_to_cpus(i, cpu_list) != 0) {
        cerr << "getNumNUMACPUs(): failed to get cpus from the numa node, errno:" << std::strerror(errno) << endl;
        exit(-1);
    }

    for (auto j = 0; j < ncores; ++j) {
        if (numa_bitmask_isbitset(cpu_list, j)) {
            ++ret;
        }
    }
    return ret;
#else
    return -1;
#endif
}

void getNUMACPUList(vector<vector<int>>& numa_cpu_list) {
    numa_cpu_list.clear();
#ifdef DNUMA
    if (numa_available() == -1) {
        cerr << "getNUMACPUList(): numa is not available.\n";
        exit(-1);
    }

    int maxnode = getNumNUMAs();
    assert(maxnode >= 1);
    unsigned int ncores = std::thread::hardware_concurrency();
    numa_cpu_list.resize(maxnode);

    vector<struct bitmask*> cpu_list;
    cpu_list.reserve(maxnode+1);
    for (auto i = 0; i < maxnode; i++) {
        cpu_list.push_back(numa_allocate_cpumask());
        if (numa_node_to_cpus(i, cpu_list.at(i)) != 0) {
            cerr << "getNUMACPUList(): failed to get cpus from numa nodes, errno:" << std::strerror(errno) << endl;
            exit(-1);
        }

        //auto nbytes = numa_bitmask_nbytes(cpu_list.at(i));
        //auto bits = nbytes * 8;
        //cout << "bits = " << bits << endl;

        for (auto j = 0; j < ncores; ++j) {
            /** output the binary cpu list info **/
            //cout << numa_bitmask_isbitset(cpu_list.at(i), j) << " | ";
            if (numa_bitmask_isbitset(cpu_list.at(i), j)) {
                numa_cpu_list.at(i).push_back(j);
            }
        }
    }
#endif
}
