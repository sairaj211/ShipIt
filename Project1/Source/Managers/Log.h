#pragma once
#include "pch.h"
#include "Core.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
/*!
 * 
 * 
 */
class Log
{
public:
	/*!
	 * 
	 * 
	 */
	GEAR_API Log();
	/*!
	 * 
	 * 
	 */
	GEAR_API ~Log();
	/*!
	 * 
	 * 
	 */
	GEAR_API static void Init();
	/*!
	 * 
	 * 
	 * \return 
	 */
	GEAR_API inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return consoleLogger; }
private:
 	static std::shared_ptr<spdlog::logger> consoleLogger;/*!<    */
	static std::vector<spdlog::sink_ptr> sinks;/*!<    */
};

// Log macros
#define LOG_TRACE(...)    ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Log::GetCoreLogger()->critical(__VA_ARGS__)

