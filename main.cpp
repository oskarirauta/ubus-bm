#include <iostream>

#include "constants.hpp"
#include "bandwidth.hpp"
#include "cmdparser.hpp"
#include "logger.hpp"
#include "loop.hpp"
#include "ubus.hpp"
#include "ubus_funcs.hpp"

bandwidth::monitor *bm;

static void version_header(void) {

	 std::cout << APP_NAME << " version " << APP_VERSION << "\n" <<
		"ubus bandwidth monitor\n" <<
		"author: Oskari Rauta" << std::endl;
}

static void usage(const CmdParser::Arg &arg) {

	std::cout << "\nusage: " << arg.cmd << " [args]" << "\n" << std::endl;
	std::cout << "options:\n" <<
		" -h, --h                usage\n" <<
		" -s, --s <socket>       Set the ubus socket to connect to\n" <<
		" -v, --v                verbose logging\n" <<
		" -d, --d                debug level logging\n" <<
		" -version, --version    show version information only\n" <<
		std::endl;
}

int main(int argc, char **argv) {

	std::string sock_fn = "";
	version_header();

	logger::loglevel(logger::info);

	CmdParser cmdparser(argc, argv,
		{
			{{ "-version", "--version" }, [](const CmdParser::Arg &arg) { exit(0); }},
			{{ "-h", "--h", "-help", "--help", "-usage", "--usage" }, [](const CmdParser::Arg &arg) {
				usage(arg);
				exit(0);
			}},
			{{ "-s", "--s", "-socket", "--socket" }, [&sock_fn](const CmdParser::Arg &arg) {
				sock_fn = arg.var;
				if ( access(sock_fn.c_str(), F_OK) == -1 ) {
					std::cout << "\nFailed to connect to ubus socket. Socket " << sock_fn << " does not exist or is not accessible." << std::endl;
					exit(1);
				}
			}, true },
			{{ "-v", "--v", "-verbose", "--verbose" }, [](const CmdParser::Arg &arg) {
				logger::loglevel(logger::vverbose);
			}},
			{{ "-d", "--d", "-debug", "--debug" }, [](const CmdParser::Arg &arg) {
				logger::loglevel(logger::debug);
			}},
			{{ "" }, [](const CmdParser::Arg &arg) {
				std::cout << "unknown argument " << arg.arg << "\n" <<
						"Try executing " << arg.cmd << " --h for usage" <<
						std::endl;
				exit(1);
			}}
		});

	cmdparser.parse();

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
		srv = new ubus::service(sock_fn);
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
	logger::info << "exiting" << std::endl;

	return 0;
}
