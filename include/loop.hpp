#pragma once

#include <mutex>
#include <chrono>
#include "constants.hpp"

struct Loop {

	private:
		bool _sig_exit;
		bool _running;
		int _delay;
		std::chrono::seconds _next_cycle;
		std::mutex sig_mutex;

		bool trigger_speedtest;

		void sleep(int ms);

	public:
		bool sig_exit(void);
		bool running(void);
		int delay(void);

		inline std::chrono::seconds next_cycle(void) {
			return this -> _next_cycle;
		}

		void set_sig_exit(bool state);
		void set_running(bool state);
		void set_delay(int delay);

		void run(void);

		Loop() : _sig_exit(false), _running(false), _delay(DEFAULT_DELAY),
			_next_cycle(std::chrono::seconds(0)),
			trigger_speedtest(false) {}
};

extern Loop main_loop;

void run_main_loop(void);
