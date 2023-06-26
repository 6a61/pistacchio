#pragma once

#include <cstdint>
#include <ctime>

class Time
{
public:
	static const std::timespec starting_time;

	// Get current time since epoch in standard timespec format.
	static std::timespec now();

	// Get number of seconds elapsed since the beginning of the program.
	static double seconds();

	// Get number of seconds elapsed since a certain time `t`.
	static double seconds_since(const std::timespec& t);

	// Get number of nanoseconds elapsed since the beginning of the program.
	static uint64_t nanoseconds();

	// Get number of nanoseconds elapsed since a certain time `t`.
	static uint64_t nanoseconds_since(const std::timespec& t);
};
