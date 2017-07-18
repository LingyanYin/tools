//
// Created by lingyan on 7/14/17.
//

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <bitset>
#include <thread>

#include "../src/affinitizer.hpp"

void threadEntry(int i) {
    affinitize(i);
    assert(sched_getcpu() == i);
}

int main(int argc, char** argv) {
    vector<vector<int>> numa_cpu_list;
    getNUMACPUList(numa_cpu_list);
    for (auto i = 0; i < numa_cpu_list.size(); ++i) {
        cout << "numa node " << i << " cpu list: ";
        for (auto j = 0; j < numa_cpu_list.at(i).size(); ++j) {
            cout << " " << numa_cpu_list.at(i).at(j);
        }
        cout << endl;
    }

    vector<thread> threads;
    for (auto i = 0; i < 2; ++i) {
        for (auto j = 0; j < 8; j++) {
            threads.push_back(thread(threadEntry, j));
        }
    }
    for (auto i = 0; i < 16; ++i) {
        threads[i].join();
    }

    int numas = getNumNUMAs();
    if (numas != -1) {
        cout << "There are " << numas << " NUMA nodes.\n";
    } else {
        cout << "No NUMA defined.\n";
    }

    for (auto i = 0; i < numas; ++i) {
        cout << "NUMA " << i << " has " << getNumNUMACPUs(i) << " CPUs.\n";
    }

    return 0;
}
