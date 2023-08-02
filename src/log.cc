#include <ctime>
#include <iostream>

#include "pistacchio/log.hh"

namespace color {
	const std::string reset  = "\033[0m";
	const std::string red    = "\033[31m";
	const std::string green  = "\033[32m";
	const std::string yellow = "\033[33m";
	const std::string purple = "\033[35m";
}

std::unordered_map<std::string, uint32_t> Log::s_filters;

std::string timestamp()
{
	std::time_t time = std::time(nullptr);
	std::tm* local = std::localtime(&time);
	char timestamp[] = "YYYY-mm-dd HH:MM:SS";
	std::strftime(timestamp, sizeof(timestamp)/sizeof(char), "%Y-%m-%d %H:%M:%S", local);

	return timestamp;
}

Log::Log(const std::string& id) :
	m_id{ id }
{
}

void Log::filter(const std::string& id, uint32_t level)
{
	s_filters[id] |= level;
}

void Log::filter(uint32_t level)
{
	s_filters[m_id] |= level;
}

void Log::info(const std::string& message)
{
	if (s_filters.contains(m_id) && (s_filters[m_id] & Log::INFO))
		return;

	std::string out = timestamp() + " INFO  ";

	if (m_id.length() > 0)
		out += "[" + color::green + m_id + color::reset + "] ";

	out += message + "\n";

	std::cout << out;
}

void Log::error(const std::string& message)
{
	if (s_filters.contains(m_id) && (s_filters[m_id] & Log::ERROR))
		return;

	std::string out = timestamp() + color::red + " ERROR " + color::reset;

	if (m_id.length() > 0)
		out += "[" + color::green + m_id + color::reset + "] ";

	out += message + "\n";

	std::cout << out;
}

void Log::debug(const std::string& message)
{
	if (s_filters.contains(m_id) && (s_filters[m_id] & Log::DEBUG))
		return;

	std::string out = timestamp() + color::purple + " DEBUG " + color::reset;

	if (m_id.length() > 0)
		out += "[" + color::green + m_id + color::reset + "] ";

	out += message + "\n";

	std::cout << out;
}

void Log::warn(const std::string& message)
{
	if (s_filters.contains(m_id) && (s_filters[m_id] & Log::WARN))
		return;

	std::string out = timestamp() + color::yellow + " WARN  " + color::reset;

	if (m_id.length() > 0)
		out += "[" + color::green + m_id + color::reset + "] ";

	out += message + "\n";

	std::cout << out;
}
