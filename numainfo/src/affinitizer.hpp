//
// Created by lingyan on 7/14/17.
//

#ifndef TOOLS_AFFINITIZER_HPP
#define TOOLS_AFFINITIZER_HPP

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

// Before any other calls in this library can be used numa_available() must be called.
// If it returns -1, all other functions in this library are undefined.
/**
 * if numa_available() != -1; we can use numa
 */
#ifdef DNUMA
#include <numa.h>
#include <numaif.h>
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::thread;

void affinitize(int cpu_id);
int getNumNUMAs();
int getNumNUMACPUs(int i);
void getNUMACPUList(vector<vector<int>>& cpu_list);

#endif //TOOLS_AFFINITIZER_HPP
