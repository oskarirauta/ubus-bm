#pragma once

#include <mutex>
#include "bandwidth.hpp"

extern bandwidth::monitor *bm;
extern std::mutex bm_mutex;
extern bool bm_ok;

bool bm_update(void);
