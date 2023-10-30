#include "logger.hpp"
#include "loop.hpp"

std::mutex bm_mutex;

static void main_loop_exec(struct uloop_timeout* t) {

	logger::debug << "cycle counter triggered" << std::endl;

	std::lock_guard<std::mutex> guard(bm_mutex);
	if ( !bm -> update()) {
		logger::error << "cannot access /proc/net/dev. Abort." << std::endl;
		uloop_end();
	} else uloop_timeout_set(t, 1000);
}

struct uloop_timeout main_loop { .cb = main_loop_exec };
