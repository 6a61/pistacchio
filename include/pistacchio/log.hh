#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

// Class that provides methods useful for logging and supports filters.
// Example output: `2023-03-15 20:37:50 DEBUG [My module] Hello, world.`
class Log {
public:
	enum level {
		INFO  = 1 << 0,
		WARN  = 1 << 1,
		ERROR = 1 << 2,
		DEBUG = 1 << 3,
	};
public:
	Log(const std::string& id);

	void info(const std::string& message);
	void debug(const std::string& message);
	void warn(const std::string& message);
	void error(const std::string& message);

	void filter(uint32_t level);

private:
	std::string m_id;

//
// Static interface
//
public:
	static void filter(const std::string& id, uint32_t level);
private:
	static std::unordered_map<std::string, uint32_t> s_filters;
};
