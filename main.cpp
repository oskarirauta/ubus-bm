#include <iostream>
#include <thread>

#include "constants.hpp"
#include "bandwidth.hpp"
#include "shared.hpp"
#include "signal.hpp"
#include "logger.hpp"
#include "loop.hpp"
#include "ubus.hpp"
#include "ubus_funcs.hpp"

bandwidth::monitor *bm;
std::mutex bm_mutex;
bool bm_ok;

bool bm_update(void) {
	std::lock_guard<std::mutex> guard(bm_mutex);
	bm_ok = bm -> update();
	return bm;
}

static void die_handler(int signum) {

	if ( logger::output_level[logger::type::verbose] ||
		logger::output_level[logger::type::vverbose] ||
		logger::output_level[logger::type::debug] )
		logger::info << "received " << Signal::string(signum) << " signal" << std::endl;
	else logger::info << "received TERM signal" << std::endl;

	if ( !uloop_cancelled ) {
		logger::verbose << "stopping ubus service" << std::endl;
		uloop_end();
	}
}

int main(const int argc, const char **argv) {

	logger::output_level[logger::type::info] = true;
	logger::output_level[logger::type::error] = true;
	logger::output_level[logger::type::verbose] = true;
	logger::output_level[logger::type::vverbose] = true;
	logger::output_level[logger::type::debug] = true;

	bm = new bandwidth::monitor;
	if ( !bm_update()) {
		logger::error << "cannot access /proc/net/dev. Abort." << std::endl;
		delete bm;
		return 1;
	}

	std::cout << "ubus bandwidth monitor" << std::endl;

	Signal::register_handler(die_handler);

	uloop_init();
	ubus::service *srv;

	try {
		srv = new ubus::service;
	} catch ( ubus::exception &e ) {
		logger::error << e.what() << std::endl;
		delete bm;
		return e.code();
	}

	try {
		srv -> add_object("network.bandwidth", {

			UBUS_HANDLER("get", ubus_get),
			UBUS_HANDLER("list", ubus_list),
		});

	} catch ( ubus::exception &e ) {
		logger::error << "failed to add ubus object: " <<
				e.what() << " (code " << e.code() << ")" << std::endl;
		delete bm;
		return e.code();
	}

	logger::verbose << "starting main loop" << std::endl;
	std::thread loop_thread(run_main_loop);

	logger::verbose << "starting ubus service" << std::endl;
	uloop_run();

	uloop_done();
	delete srv;

	logger::verbose << "ubus service has stopped" << std::endl;
	logger::vverbose << "exiting main loop" << std::endl;

	main_loop.set_sig_exit(true);

	while ( main_loop.running())
		std::this_thread::sleep_for(std::chrono::milliseconds(SIG_DELAY));

	logger::vverbose << "main loop stopped" << std::endl;

	loop_thread.join();

	delete bm;
	logger::vverbose << "exiting" << std::endl;

	return 0;
}
