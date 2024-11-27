#pragma once

#include <filesystem>

struct PathedConfigRule {
	std::filesystem::path source;
	std::filesystem::path destination;
	std::string extension;
};