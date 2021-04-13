#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using options_storage = std::vector<std::pair<std::string, std::string>>;

options_storage ParseConfig(const std::string filename) {
	options_storage storage;

	std::ifstream fi(filename);

	std::string line;
	if (fi) {
		while (std::getline(fi, line))
		{
			std::istringstream iss(line);
			std::string key;
			if (std::getline(iss, key, '='))
			{
				std::string value;
				if (std::getline(iss, value))
					storage.emplace_back(key, value);
			}
		}
	}
	else {
		std::cerr << "Config file " << filename << " not opened.\n";
	}
	return storage;
}