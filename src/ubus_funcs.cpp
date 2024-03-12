#include "json.hpp"
#include "loop.hpp"
#include "logger.hpp"
#include "ubus_funcs.hpp"

int ubus_get(const std::string& method, const std::string& msg, std::string& result) {

	std::string ifd_name;

	logger::debug["ubus::get"] << "ubus call received" << std::endl;

	if ( !msg.empty()) {

		try {

			JSON json = JSON::parse(msg);
			if ( json.contains("interface") && json["interface"].is_convertible(JSON::TYPE::STRING))
				ifd_name = json["interface"].to_string();

		} catch ( const JSON::exception& e ) {

			ifd_name = "";
			logger::error["ubus::get"] << "failed to parse json message" << logger::detail(e.what()) << std::endl;
		}
	}

	logger::vverbose["ubus::get"] << "called for interface " << ( ifd_name.empty() ? "(empty)" : ifd_name ) << std::endl;

	JSON json;
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

		json[ifd.name()] = JSON::Object({
			{ "rx", JSON::Object({
				{ "bytes", (long long)rx_bytes },
				{ "kilobytes", (long long)(rx_bytes / 1024) },
				{ "megabytes", (long long)((rx_bytes / 1024) / 1024) },
				{ "packets", (long long)ifd.rx_packets() },
				{ "errors", (long long)ifd.rx_errors() },
				{ "rate", JSON::Object({
					{ "Bytes", (long long)rx },
					{ "KBytes", (long long)(rx / 1024) },
					{ "MBytes", (long long)((rx / 1024) / 1024) },
					{ "b", (long long)rxM },
					{ "k", (long long)(rxM / 1024) },
					{ "m", (long long)((rxM / 1024) / 1024) }
				})}
			})},
			{ "tx", JSON::Object({
				{ "bytes", (long long)tx_bytes },
				{ "kilobytes", (long long)(tx_bytes / 1024) },
				{ "megabytes", (long long)((tx_bytes / 1024) / 1024) },
				{ "packets", (long long)ifd.tx_packets() },
				{ "errors", (long long)ifd.tx_errors() },
				{ "rate", JSON::Object({
					{ "Bytes", (long long)tx },
					{ "KBytes", (long long)(tx / 1024) },
					{ "MBytes", (long long)((tx / 1024) / 1024) },
					{ "b", (long long)txM },
					{ "k", (long long)(txM / 1024) },
					{ "m", (long long)((txM / 1024) / 1024) }
				})}
			})}
		});
	}

	if ( !found ) {

		json["error"] = "interface not found";
		logger::debug["ubus::get"] << "call for interface " << ifd_name << " error, interface not found" << std::endl;
	}

	result = json.dump(false);
	return 0;
}

int ubus_list(const std::string& method, const std::string& msg, std::string& result) {

	logger::debug["ubus::list"] << "ubus call received" << std::endl;

	JSON json;
	std::lock_guard<std::mutex> guard(bm_mutex);

	for ( auto &ifd : bm -> interfaces()) {

		uint64_t rx_bytes = ifd.rx_bytes();
		uint64_t tx_bytes = ifd.tx_bytes();
		uint64_t rx = ifd.rx_rate();
		uint64_t tx = ifd.tx_rate();
		uint64_t rxM = rx * 8;
		uint64_t txM = tx * 8;

		json[ifd.name()] = JSON::Object({
			{ "rx", JSON::Object({
				{ "bytes", (long long)rx_bytes },
				{ "kilobytes", (long long)(rx_bytes / 1024) },
				{ "megabytes", (long long)((rx_bytes / 1024) / 1024) },
				{ "packets", (long long)ifd.rx_packets() },
				{ "errors", (long long)ifd.rx_errors() },
				{ "rate", JSON::Object({
					{ "Bytes", (long long)rx },
					{ "KBytes", (long long)(rx / 1024) },
					{ "MBytes", (long long)((rx / 1024) / 1024) },
					{ "b", (long long)rxM },
					{ "k", (long long)(rxM / 1024) },
					{ "m", (long long)((rxM / 1024) / 1024) }
				})}
			})},
			{ "tx", JSON::Object({
				{ "bytes", (long long)tx_bytes },
				{ "kilobytes", (long long)(tx_bytes / 1024) },
				{ "megabytes", (long long)((tx_bytes / 1024) / 1024) },
				{ "packets", (long long)ifd.tx_packets() },
				{ "errors", (long long)ifd.tx_errors() },
				{ "rate", JSON::Object({
					{ "Bytes", tx },
					{ "KBytes", (long long)(tx / 1024) },
					{ "MBytes", (long long)((tx / 1024) / 1024) },
					{ "b", txM },
					{ "k", (long long)(txM / 1024) },
					{ "m", (long long)((txM / 1024) / 1024) }
				})}
			})}
		});

	}

	result = json.dump(false);
	return 0;
}
