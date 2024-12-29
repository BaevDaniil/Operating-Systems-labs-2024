#pragma once

#include <vector>
#include <filesystem>
#include <optional>
#include <fstream>
#include "ConfigRule.h"

class Config {
private:
	std::filesystem::path path;
	void checkAbsolute();
	void logNonexistentFile();
	std::ifstream openSourceSafely();
	std::optional<ConfigRule> parseLine(std::string const& line);
public:
	Config() = default; // to construct Daemon
	Config(std::string const& path);
	std::vector<ConfigRule> getRules();
};