#include <chrono>
#include <cstdint>
#include <exception>
#include <stdint.h>
#include <thread>

#include "pistacchio/app.hh"
#include "pistacchio/time.hh"

App::App(double fixed_rate, double target_rate = 0.0) :
	m_fixed_rate(fixed_rate),
	m_target_rate(target_rate)
{
}

App::~App()
{
}

void App::start()
{
	m_stop = false;

	uint64_t dt = (1.0/m_fixed_rate) * 1e+9;
	uint64_t accumulator = 0;
	uint64_t current_time = Time::nanoseconds();
	//auto& state_manager = state::manager::instance();

	while (!m_stop) {
		uint64_t new_time = Time::nanoseconds();
		uint64_t frame_time = new_time - current_time;
		current_time = new_time;

#if 0
		// Limit frame_time to avoid spiral of death due to slow
		// processing and attempting to catch up

		if (frame_time > 0.25)
			frame_time = 0.25;
#endif

		accumulator += frame_time;

		//state_manager.update();
		//state_manager.do_states_update(frame_time / 1e+9);
		update(frame_time / 1e+9);

		while (accumulator >= dt) {
			//state_manager.do_states_fixed_update(dt / 1e+9);
			fixed_update(dt / 1e+9);
			m_time += dt;
			accumulator -= dt;
		}

		double alpha = static_cast<double>(accumulator) / static_cast<double>(dt);
		//state_manager.do_states_render(alpha);
		render(alpha);

		// FPS limiter

		if (m_target_rate > 0) {
			uint64_t end_time = Time::nanoseconds();
			uint64_t target_period = (1.0/m_target_rate) * 1e+9;

			if (end_time - current_time < target_period) {
				uint64_t wait_time = target_period - (end_time - current_time);
				std::this_thread::sleep_for(std::chrono::duration<uint64_t, std::nano>{wait_time});
			}
		}
	}
}

void App::stop()
{
	m_stop = true;
}

void App::fixed_rate(double value)
{
	m_fixed_rate = value;
}

void App::target_rate(double value)
{
	m_target_rate = value;
}

void App::update(double) {}
void App::fixed_update(double) {}
void App::render(double) {}
