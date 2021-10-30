#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>


std::vector<std::string> getAllOptions(std::string line) {
	std::istringstream iss(line);
	std::vector<std::string> result;
	std::string str;
	while (iss >> str) {
		result.push_back(str);
	}

	return result;
}

// parser na range
bool getRanges(std::string& string, size_t& from, size_t& to, size_t max) {
	size_t commaI = string.find(',', 0);
	if (commaI == string.npos) {
		if (string.find_first_not_of("0123456789") != string.npos) {
			return false;
		}
		from = to = std::stoull(string);
		if (from == 0) {
			return false;
		}
		from -= 1;
		return true;
	}
	else {
		if (string.find(',', commaI + 1) != string.npos || string.find_first_not_of(",0123456789") != string.npos) {
			return false;
		}
		std::string num1 = string.substr(0, commaI);
		std::string num2 = string.substr(commaI + 1, string.length() - commaI - 1);
		if (!num1.empty()) {
			from = std::stoull(num1);
		}
		if (!num2.empty()) {
			to = std::stoull(num2);
		}
		else {
			to = max;
		}
		if (from == 0 || to == 0) {
			return false;
		}
		return true;
	}
}


void insertInto(std::vector<std::string>& data, std::vector<std::string> options) {
	std::string inWrite;
	if (options.size() == 1) {
		std::getline(std::cin, inWrite);
		while (inWrite.size() != 1 && inWrite[0] != '.') {
			data.push_back(inWrite);
			std::getline(std::cin, inWrite);
		}
	}
	else if (options.size() == 2) {
		if (options[1].find_first_not_of("0123456789") != options[1].npos) {
			return;
		}
		size_t line = std::stoull(options[1]);
		if (line == 0) {
			return;
		}
		line--;
		std::vector<std::string> insert;
		std::getline(std::cin, inWrite);
		while (inWrite.size() != 1 && inWrite[0] != '.') {
			insert.push_back(inWrite);
			std::getline(std::cin, inWrite);
		}
		data.insert(data.begin() + line, insert.begin(), insert.end());
	}
	else {

	}
}


void deleteLines(std::vector<std::string>& data, std::vector<std::string> options) {
	size_t from = 0, to = 0;
	if (options.size() == 2) {
		from = 1;
		if (!getRanges(options[1], from, to, data.size())) {
			return;
		};
		from -= 1;
	}
	if (from > to) {
		std::cout << "Invalid range" << std::endl;
		return;
	}
	auto last = (data.size() > to) ? (data.begin() + to) : data.end();

	data.erase(data.begin() + from, last);
}

void printFromTo(std::vector<std::string>& data,std::vector<std::string> options) {
	size_t from = 0, to = data.size();
	if (options.size() == 2) {
		from = 1;
		if (!getRanges(options[1], from, to, data.size())) {
			return;
		};
		from -= 1;
	}
	if (from > to) {
		std::cout << "Invalid range" << std::endl;
		return;
	}
	for (auto it = data.begin() + from; it != data.begin() + to; it++) {
		std::cout << *it << std::endl;
	}
}


//TODO: lepsie overovanie
void writeChangesToFile(std::string filename,std::vector<std::string> workString) {
	std::ofstream file(filename);
	
	for (auto& str : workString) {
		file << str << std::endl;
	}

	file.close();
}


bool hasUnsaved(std::string filename, std::vector<std::string> workString) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		return true;
	}
	std::string buffer;
	size_t index = 0;
	while (std::getline(file, buffer)) {
		if (workString.size() == index) {
			return true;
		}
		if (buffer.compare(workString[index])) {
			file.close();
			return true;
		}
		index++;
	}

	file.close();

	if (index != workString.size()) {
		return true;
	}
	return false;
}



int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "neplatny pocet argumentov" << std::endl;
		return EXIT_FAILURE;
	}
	std::vector<std::string> fileContent;
	std::string filename = argv[1];

	std::ifstream readFile(filename);
	if (readFile.is_open()) {
		const size_t readSize = 4096;
		std::string buffer;
		while (std::getline(readFile, buffer)) {
			fileContent.push_back(buffer);
		}

		if (!readFile.eof()) {
			std::cerr << "Error reading file" << std::endl;
			return EXIT_FAILURE;
		}

		readFile.close();
	}

	bool close = false;

	while (!close) {
		std::cout << "* ";
		std::string vstup;
		std::getline(std::cin, vstup);
		auto options = getAllOptions(vstup);
		std::string prepinacStr = options[0];

		if ((prepinacStr.size() != 1  && prepinacStr.compare("q!"))) {
			std::cout << "Unsupported command" << std::endl;
		}
		else {
			bool over = false;
			switch (prepinacStr[0]) {
			case 'w':
				writeChangesToFile(filename, fileContent);
				break;
			case 'q':
				if (!prepinacStr.compare("q!")) {
					over = true;
				}
				if (over || !hasUnsaved(filename, fileContent)) {
					close = true;
				}
				else {
					std::cout << "You have unsaved changes" << std::endl;
				}
				break;
			case 'a':
				insertInto(fileContent, options);
				break;
			case 'p':
				printFromTo(fileContent, options);
				break;
			case 'd':
				deleteLines(fileContent, options);
				break;
			default:
				std::cout << "Unsupported command" << std::endl;
				break;
			}
		}


	}
	return EXIT_SUCCESS;
}
