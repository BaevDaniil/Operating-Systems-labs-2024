#include <fstream>
#include <optional>
#include <iostream>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <syslog.h>
#include "Config.h"

using std::ifstream;
using std::string;
using std::cout;
using std::endl;
using std::optional;
using std::vector;
namespace fs = std::filesystem;

enum class ReadState {
	Reading,
	ReadWhitespace,
};

ifstream Config::openSourceSafely() {
	this->logNonexistentFile();
	ifstream source(this->path);
	if (!source.is_open()) {
		syslog(LOG_ERR, "Could not open config file %s.", this->path.c_str());
		exit(EXIT_FAILURE);
	}
	syslog(LOG_INFO, "Start reading config file %s.", this->path.c_str());
	return source;
}

void Config::checkAbsolute() {
	if (!this->path.is_absolute()) {
		this->path = fs::absolute(
			fs::current_path() / this->path
		);
	}
}

void Config::logNonexistentFile() {
	//cout << "path:" << this->path << endl;
	if (!std::filesystem::exists(this->path)) {
		//cout << "Could not locate config file " << this->path << endl;
		syslog(LOG_ERR, "Could not locate config file %s.", this->path.c_str());
	}
}

Config::Config(string const& path) {
	this->path = fs::path(path);
	// Check if relative or absolute
	this->checkAbsolute();
}

inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

optional<ConfigRule> Config::parseLine(string const& line) {
	vector<string> parts;
	string current;
	string trail;
	ReadState state = ReadState::Reading;
	size_t size = line.size();
	for (size_t i = 0; i < size; ++i) {
		char ch = line[i];
		if (ch == ' ') {
			state = ReadState::ReadWhitespace;
			current += ch;
		}
		else {
			if (state == ReadState::Reading) {
				current += ch;
			}
			else if ((state == ReadState::ReadWhitespace) && (ch == '/')) {
				rtrim(current);
				parts.push_back(current);
				state = ReadState::Reading;
				current = ch;
			}
			else {
				current += ch;
				state = ReadState::Reading;
			}
		}
	}
	size_t whitespace = current.find_last_of(' ');
	if (whitespace != string::npos) {
		parts.push_back(current.substr(0, whitespace));
		parts.push_back(current.substr(whitespace + 1));
	}
	else {
		return {};
	}
	if (parts.size() == 3) {
		ConfigRule rule;
		rule.source = parts[0].substr(1);
		rule.destination = parts[1].substr(1);
		rule.extension = parts[2];
		syslog(LOG_INFO, "Read line %s,%s,%s", rule.source.c_str(),
			rule.destination.c_str(), rule.extension.c_str());
		return rule;
	}
	else {
		return {};
	}
}

vector<ConfigRule> Config::getRules() {
	ifstream source = this->openSourceSafely();
	//cout << "inside " << endl;
	string line;
	vector<ConfigRule> rules;

	while (std::getline(source, line)) {
		//rules.push_back(this->parseLine(line));
		//cout << line << endl;
		optional<ConfigRule> rule = this->parseLine(line);
		if (rule.has_value()) {
			rules.emplace_back(rule.value());
		}
		else {
			syslog(LOG_ERR, "Could not parse this line: %s", line.c_str());
		}
	}
	source.close();
	syslog(LOG_INFO, "Finished reading config file %s.", this->path.c_str());
	return rules;
}

