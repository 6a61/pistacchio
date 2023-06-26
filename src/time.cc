#include "pistacchio/time.hh"

const std::timespec Time::starting_time = Time::now();

std::timespec Time::now()
{
	std::timespec ret;

	std::timespec_get(&ret, TIME_UTC);

	return ret;
}

double Time::seconds()
{
	std::timespec now = Time::now();

	double seconds = now.tv_sec - Time::starting_time.tv_sec;
	double nanoseconds = (now.tv_nsec - Time::starting_time.tv_nsec) * 1e-9;

	return seconds + nanoseconds;
}

double Time::seconds_since(const std::timespec& t)
{
	std::timespec now = Time::now();

	double seconds = now.tv_sec - t.tv_sec;
	double nanoseconds = (now.tv_nsec - t.tv_nsec) * 1e-9;

	return seconds + nanoseconds;
}

uint64_t Time::nanoseconds()
{
	std::timespec now = Time::now();

	uint64_t seconds = (now.tv_sec - Time::starting_time.tv_sec) * 1e+9;
	uint64_t nanoseconds = now.tv_nsec - Time::starting_time.tv_nsec;

	return seconds + nanoseconds;
}

uint64_t Time::nanoseconds_since(const std::timespec& t)
{
	std::timespec now = Time::now();

	uint64_t seconds = (now.tv_sec - t.tv_sec) * 1e+9;
	uint64_t nanoseconds = now.tv_nsec - t.tv_nsec;

	return seconds + nanoseconds;
}
