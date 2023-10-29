#pragma once

#include <mutex>
#include "bandwidth.hpp"

extern bandwidth::monitor *bm;
extern std::mutex bm_mutex;
extern bool bm_ok;
extern pid_t main_pid;

bool bm_update(void);
