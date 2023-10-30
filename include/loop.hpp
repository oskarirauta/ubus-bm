#pragma once

#include <mutex>
#include "ubus.hpp"
#include "bandwidth.hpp"

extern bandwidth::monitor *bm;
extern std::mutex bm_mutex;
extern struct uloop_timeout main_loop;
