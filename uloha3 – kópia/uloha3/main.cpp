#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>


std::vector<std::string> readLinesFromFile(std::ifstream& file) {
	std::vector<std::string> result;
	std::string buffer;
	/*while (std::getline(file, buffer)) {
		result.push_back(buffer);
	}*/
	buffer.resize(1000);
	std::string res = "";
	while (file.read(buffer.data(),buffer.size())) {
		res.append(buffer);
	}
	size_t shrink = file.gcount();
	if (shrink > 0) {
		buffer.resize(shrink);
		res.append(buffer);
	}
	size_t from = 0, to = res.find('\n',from);
	bool empty = true;
	while (to != res.npos) {
		result.push_back(res.substr(from, to-from+1));
		from = to + 1;
		to = res.find('\n', from);
		empty = false;
	}
	if (!empty)
		result.push_back(res.substr(from, res.length() - from));
	return result;
}

std::vector<std::string> getAllOptions(std::string line) {
	std::stringstream iss(line);
	std::vector<std::string> result;
	std::string str;
	while (iss >> str){
		result.push_back(str);
		if (result.size() == 3) break;
	}
	result.push_back(line);

	return result;
}
//false - string
//true - Range
bool stringOrRange(std::string& string) {
	size_t comma = string.find(',');
	if (comma == string.npos) {
		if (string.find_first_not_of("01234567890") != string.npos) {
			return false;
		}
		return true;
	}
	else {
		if (string.find(',', comma + 1) != string.npos) {
			return false;
		}
		if (string.find_first_not_of("0123456789,") != string.npos) {
			return false;
		}
		return true;
	}
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
		from -= 1;
		return true;
	}
}

void writeMode(std::vector<std::string>& data) {
	std::string inWrite;
	std::getline(std::cin, inWrite);
	while (inWrite.size() != 1 || inWrite[0] != '.') {
		data.push_back(inWrite);
		std::getline(std::cin, inWrite);
	}
}


void insertInto(std::vector<std::string>& data, std::vector<std::string> options, bool change) {
	if (options.size() == 2) {
		writeMode(data);
	}
	else if (options.size() == 3 && ((change && stringOrRange(options[1])) || options[1].find_first_not_of("0123456789") == options[1].npos)) {
		size_t line = 0;
		if (change) {
			size_t from = 0, to = data.size();
			if (!getRanges(options[1], from, to, data.size())) {
				return;
			}
			line = from;
		}
		else if(options[1].find_first_not_of("0123456789") == options[1].npos) {
			line = std::stoull(options[1]);
			/*if (line == 0) {
				return;
			}*/
		}
		std::vector<std::string> insert;
		writeMode(insert);
		if (data.size() < line) {
			size_t size = line - data.size();
			for (size_t i = 0; i <size; i++) {
				data.push_back("");
			}
		}
		data.insert(data.begin() + line, insert.begin(), insert.end());
	}
	else {
		if (options.size() == 3) {
			data.push_back(options[1]);
		}
		else {
			bool range = false;
			size_t line = data.size();
			if (change && stringOrRange(options[1])) {
				size_t from = 0, to = data.size();
					if (!getRanges(options[1], from, to, data.size())) {
						return;
					}
				line = from;
				range = true;
			} 
			else if (options[1].find_first_not_of("0123456789") == options[1].npos){
				line = std::stoull(options[1]);
				/*if (line == 0) {
					return;
				}*/
				range = true;
			}
			std::vector<std::string> insert;
			size_t oneLinerStart = options[3].find_first_of(" \t");
			if (range) {
				oneLinerStart = options[3].find_first_of(" \t", oneLinerStart + 1);
			}
			insert.push_back(options[3].substr(oneLinerStart+1,options[3].size() - oneLinerStart));
			if (data.size() < line) {
				size_t size = line - data.size();
				for (size_t i = 0; i < size; i++) {
					data.push_back("");
				}
			}
			data.insert(data.begin() + line, insert.begin(), insert.end());
		}
	}
}


void deleteLines(std::vector<std::string>& data, std::vector<std::string> options, bool change) {
	size_t from = 0, to = data.size();
	if (data.empty()) return;

	if (options.size() == 3 || change) {
		if (change && stringOrRange(options[1]) || !change) {
			from = 1;
			if (!getRanges(options[1], from, to, data.size())) {
				std::cout << "Unsupported command" << std::endl;
				return;
			}
		}
	}



	if (from >= data.size()) {
		std::cerr << "Presiahnutie pola" << std::endl;
		return;
	}
	if (from > to) {
		std::cout << "Invalid range" << std::endl;
		return;
	}
	if (to > data.size()) {
		to = data.size();
	}

	auto last = (data.size() > to) ? (data.begin() + to) : data.end();

	data.erase(data.begin() + from, last);
}

void printFromTo(std::vector<std::string>& data,std::vector<std::string> options) {
	size_t from = 0, to = data.size();
	if (data.empty()) return;

	if (options.size() == 3) {
		from = 1;
		if (!getRanges(options[1], from, to, data.size())) {
			std::cout << "Unsupported command" << std::endl;
			return;
		};
	}
	if (from > to) {
		std::cout << "Invalid range" << std::endl;
		return;
	}
	if (from >= data.size()) {
		//std::cerr << "Presiahnutie pola" << std::endl;
		return;
	}
	if (to > data.size()) {
		to = data.size();
	}
	std::string output = "";
	for (auto it = data.begin() + from; it != data.begin() + to; it++) {
		output+=*it;
	}
	
	std::cout << output << std::endl;
}


//TODO: lepsie overovanie
bool writeChangesToFile(std::string filename,std::vector<std::string> workString) {
	std::ofstream file(filename);
	
	if (!file.is_open()) {
		std::cerr << "nepodarilo sa ulozit do suboru";
		return false;
	}
	for (auto& str : workString) {
		file << str << std::endl;
	}

	file.close();
	return true;
}


bool hasUnsaved(std::string filename, std::vector<std::string> workString) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		return true;
	}
	std::vector<std::string> tempFileContent = readLinesFromFile(file);

	if (!file.eof()) {
		return true;
	}
	file.close();

	if (workString.size() != tempFileContent.size()) {
		return true;
	}
	if (workString != tempFileContent) {
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

	std::fstream temp;
	temp.open(filename, std::ios::out | std::ios::app);
	if (!temp.is_open()) {
		std::cerr << "Nepodarilo otvorit subor" << std::endl;
		return EXIT_FAILURE;
	}
	temp.close();

	std::ifstream readFile(filename);
	if (readFile.is_open()) {	
		
		fileContent = readLinesFromFile(readFile);

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
		if (vstup.empty()) {
			std::cout << "Unsupported command" << std::endl;
			continue;
		}
		auto options = getAllOptions(vstup);
		std::string prepinacStr = options[0];

		if ((prepinacStr.size() != 1  && prepinacStr.compare("q!"))) {
			std::cout << "Unsupported command" << std::endl;
		}
		else {
			bool over = false;
			switch (prepinacStr[0]) {
			case 'w':
				if (options.size() < 3) {
					if (!writeChangesToFile(filename, fileContent)) {
						return EXIT_FAILURE;
					}
				}
				else { std::cout << "Unsupported command" << std::endl; }
				break;
			case 'q':
				if (options.size() < 3) {
					if (!prepinacStr.compare("q!")) {
						over = true;
					}
					if (over || !hasUnsaved(filename, fileContent)) {
						close = true;
					}
					else {
						std::cout << "You have unsaved changes" << std::endl;
					}
				}
				else { std::cout << "Unsupported command" << std::endl; }
				break;
			case 'a':
				insertInto(fileContent, options,false);
				break;
			case 'p':
				if (options.size() <= 3) {
					printFromTo(fileContent, options);
				}
				else{ std::cout << "Unsupported command" << std::endl; }
				break;
			case 'd':
				if (options.size() <= 3) {
					deleteLines(fileContent, options, false);
				}
				else { std::cout << "Unsupported command" << std::endl;  }
				break;
			case 'c':
				deleteLines(fileContent, options,true);
				insertInto(fileContent, options,true);
				break;
			default:
				std::cout << "Unsupported command" << std::endl;
				break;
			}
		}


	}
	return EXIT_SUCCESS;
}
