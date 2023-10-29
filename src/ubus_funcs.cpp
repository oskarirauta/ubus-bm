#include "json.hpp"
#include "logger.hpp"
#include "shared.hpp"

#include "ubus_funcs.hpp"

int ubus_get(const std::string& method, const std::string& msg, std::string& result) {

	std::string ifd_name;

	if ( !msg.empty()) {

		json::JSON json_msg = json::JSON::Load(msg);
		if ( json_msg.size() != 0 && json_msg.hasKey("interface") && json_msg["interface"].IsString())
			ifd_name = json_msg["interface"].ToString();
	}

	logger::debug << "called ubus::" << method << " for interface " << ( ifd_name.empty() ? "(empty)" : ifd_name ) << std::endl;

	json::JSON answer;
	bool found = false;

	std::lock_guard<std::mutex> guard(bm_mutex);

	for ( auto &ifd : bm -> interfaces()) {

		if ( ifd.name() != ifd_name )
			continue;

		found = true;

		uint64_t rx_bytes = ifd.rx_bytes();
		uint64_t tx_bytes = ifd.tx_bytes();
		uint64_t rx = ifd.rx_rate();
		uint64_t tx = ifd.tx_rate();
		uint64_t rxM = rx * 8;
		uint64_t txM = tx * 8;

		answer[ifd.name()] = {
			"rx", {
				"bytes", rx_bytes,
				"kilobytes", (uint64_t)(rx_bytes / 1024),
				"megabytes", (uint64_t)((rx_bytes / 1024) / 1024),
				"packets", ifd.rx_packets(),
				"errors", ifd.rx_errors(),
				"rate", {
					"Bytes", rx,
					"KBytes", (uint64_t)(rx / 1024),
					"MBytes", (uint64_t)((rx / 1024) / 1024),
					"b", rxM,
					"k", (uint64_t)(rxM / 1024),
					"m", (uint64_t)((rxM / 1024) / 1024)
				}
			},
			"tx", {
				"bytes", tx_bytes,
				"kilobytes", (uint64_t)(tx_bytes / 1024),
				"megabytes", (uint64_t)((tx_bytes / 1024) / 1024),
				"packets", ifd.tx_packets(),
				"errors", ifd.tx_errors(),
				"rate", {
					"Bytes", tx,
					"KBytes", (uint64_t)(tx / 1024),
					"MBytes", (uint64_t)((tx / 1024) / 1024),
					"b", txM,
					"k", (uint64_t)(txM / 1024),
					"m", (uint64_t)((txM / 1024) / 1024)
				}
			}
		};
	}

	if ( !found ) {
		answer["error"] = "interface not found";
		logger::debug << "ubus::get for interface " << ifd_name << " error, interface not found" << std::endl;
	}

	result = answer.dumpMinified();
	return 0;
}

int ubus_list(const std::string& method, const std::string& msg, std::string& result) {

	logger::debug << "called ubus::" << method << std::endl;

	json::JSON json;
	std::lock_guard<std::mutex> guard(bm_mutex);

	for ( auto &ifd : bm -> interfaces()) {

		uint64_t rx_bytes = ifd.rx_bytes();
		uint64_t tx_bytes = ifd.tx_bytes();
		uint64_t rx = ifd.rx_rate();
		uint64_t tx = ifd.tx_rate();
		uint64_t rxM = rx * 8;
		uint64_t txM = tx * 8;

		json[ifd.name()] = {
			"rx", {
				"bytes", rx_bytes,
				"kilobytes", (uint64_t)(rx_bytes / 1024),
				"megabytes", (uint64_t)((rx_bytes / 1024) / 1024),
				"packets", ifd.rx_packets(),
				"errors", ifd.rx_errors(),
				"rate", {
					"Bytes", rx,
					"KBytes", (uint64_t)(rx / 1024),
					"MBytes", (uint64_t)((rx / 1024) / 1024),
					"b", rxM,
					"k", (uint64_t)(rxM / 1024),
					"m", (uint64_t)((rxM / 1024) / 1024)
				}
			},
			"tx", {
				"bytes", tx_bytes,
				"kilobytes", (uint64_t)(tx_bytes / 1024),
				"megabytes", (uint64_t)((tx_bytes / 1024) / 1024),
				"packets", ifd.tx_packets(),
				"errors", ifd.tx_errors(),
				"rate", {
					"Bytes", tx,
					"KBytes", (uint64_t)(tx / 1024),
					"MBytes", (uint64_t)((tx / 1024) / 1024),
					"b", txM,
					"k", (uint64_t)(txM / 1024),
					"m", (uint64_t)((txM / 1024) / 1024)
				}
			}
		};

	}

	result = json.dumpMinified();
	return 0;
}
