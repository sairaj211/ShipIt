#include <pch.h>
#include "Managers/Log.h"
#include "Core.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"

Log::Log()
{

}

Log::~Log()
{

}

std::shared_ptr<spdlog::logger> Log::consoleLogger;


std::vector<spdlog::sink_ptr> Log::sinks;

void Log::Init()
{
	sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_st>());
	sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>("./Resources/logs/5thGearEngine.txt", 23, 59));
	consoleLogger = std::make_shared<spdlog::logger>("5thGear", begin(sinks), end(sinks));
	spdlog::set_pattern("%^[%T][%n][%l] : %v%$");
	consoleLogger->set_level(spdlog::level::trace);

// 	//register it if you need to access it globally
// 	spdlog::register_logger(consoleLogger);
}


