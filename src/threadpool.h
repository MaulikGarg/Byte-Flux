#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "ioprocess.h"
#include "utility.h"
#include <thread>
#include <mutex>
#include <queue>
#include <vector>

class ThreadPool{
    std::vector<std::thread> m_threads;
    std::queue<IO_process> m_processes;
    std::mutex m_mutex;

};

#endif