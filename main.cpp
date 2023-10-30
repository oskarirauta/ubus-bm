#include <iostream>

#include "constants.hpp"
#include "bandwidth.hpp"
#include "logger.hpp"
#include "loop.hpp"
#include "ubus.hpp"
#include "ubus_funcs.hpp"

bandwidth::monitor *bm;

int main(const int argc, const char **argv) {

	logger::output_level[logger::type::info] = true;
	logger::output_level[logger::type::error] = true;
	logger::output_level[logger::type::verbose] = true;
	logger::output_level[logger::type::vverbose] = true;
	logger::output_level[logger::type::debug] = true;

	std::cout << APP_NAME << " version " << APP_VERSION << "\n" <<
		"ubus bandwidth monitor\n" <<
		"author: Oskari Rauta\n" << std::endl;

	bm = new bandwidth::monitor;
	if ( !bm -> update()) {
		logger::error << "cannot access /proc/net/dev. Abort." << std::endl;
		delete bm;
		return 1;
	}

	uloop_init();
	uloop_timeout_set(&main_loop, 1000);

	ubus::service *srv;

	try {
		srv = new ubus::service;
	} catch ( ubus::exception &e ) {
		logger::error << e.what() << std::endl;
		delete bm;
		return e.code();
	}

	try {
		srv -> add_object("network.usage", {

			UBUS_HANDLER("get", ubus_get),
			UBUS_HANDLER("list", ubus_list),
		});

	} catch ( ubus::exception &e ) {
		logger::error << "failed to add ubus object: " <<
				e.what() << " (code " << e.code() << ")" << std::endl;
		delete bm;
		return e.code();
	}

	logger::vverbose << "starting ubus service" << std::endl;
	uloop_run();

	uloop_done();
	delete srv;

	logger::vverbose << "ubus service has stopped" << std::endl;

	delete bm;
	logger::vverbose << "exiting" << std::endl;

	return 0;
}
