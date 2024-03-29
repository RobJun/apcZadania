#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>


std::vector<std::string> readLinesFromFile(std::ifstream& file) {
	std::vector<std::string> result;
	std::string resStr = "" , buffer = "";
	buffer.resize(1024);
	//precitaj cely subor
	while (file.read(buffer.data(), 1024)) {
		resStr += buffer;
	}
	buffer.resize(file.gcount());
	resStr += buffer;

	size_t startOfRow = 0;
	size_t newLine = resStr.find('\n');
	while (newLine != resStr.npos) {
		result.push_back(resStr.substr(startOfRow, newLine - startOfRow + 1));
		startOfRow = newLine + 1;
		newLine = resStr.find('\n',startOfRow);
	}
	if(resStr.size() - startOfRow != 0)
		result.push_back(resStr.substr(startOfRow, resStr.size()- startOfRow));

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
	if (result.size() >= 2) {
		size_t afterCommand = line.find(result[1],line.find(result[0])+1);
		result.push_back(line.substr(afterCommand, line.length() - afterCommand));
	}

 	return result;
}

// parser na range
bool getRanges(std::string& string, size_t& from, size_t& to, size_t max) {
	size_t commaI = string.find(',', 0);
	if (commaI == string.npos) { // ak range je iba jeden riadok 
		if (string.find_first_not_of("0123456789") != string.npos) {
			return false;
		}
		if (string.size() > 19) return false;
		from = to = std::stoull(string);
		if (from == 0) {
			return false;
		}
		from -= 1;
		return true;
	}
	else {	//ak je to range
		if (string.find(',', commaI + 1) != string.npos || string.find_first_not_of(",0123456789") != string.npos) {
			//obsahoval ciarky navyse alebo nieco ine ako cislo a ciarku
			return false;
		}
		std::string num1 = string.substr(0, commaI);
		std::string num2 = string.substr(commaI + 1, string.length() - commaI - 1);
		if (!num1.empty()) {
			//ak je cislo vacsie ako je mozne
			if (num1.size() > 19) return false;
			else {
				from = std::stoull(num1);
				//ak je cislo 0 tak je to invalid range
				if (from == 0) return false;
			}
		}
		if (!num2.empty()) {
			if (num2.size() > 19) return false;
			to = std::stoull(num2);
			if (to == 0) return false;
			if (from > to) {
				return false;
			}
		}
		else {to = max;}
		from -= 1;
		return true;
	}
}


void writeMode(std::vector<std::string>& data) {
	std::string inWrite;
	std::getline(std::cin, inWrite);
	while (inWrite.size() != 1 || inWrite[0] != '.') {
	//while (std::getline(std::cin, inWrite)) {
		//if (inWrite.size() == 1 && inWrite[0] == '.') return;
		if (data.size() == 1 && data.back().empty()) data.pop_back();
		//ak za poslednym riadkom nie je \n vloz ho tam
		if(!data.empty() && data.back().find('\n') == data.back().npos) data.back() += '\n';
		data.push_back(inWrite+'\n');
		std::getline(std::cin, inWrite);
	}
}


void appendToPoint(std::vector<std::string>& data, std::vector<std::string>& insert, size_t line) {
	//vkladanie prazdnych riadkov
	if (data.size() < line) {
		size_t size = line - data.size();
		for (size_t i = 0; i < size; i++) {
			//ak za poslednym riadkom nie je \n vloz ho tam
			if (!data.empty() && data.back().find('\n') == data.back().npos) data.back() += '\n';
			data.push_back("");
		}
	}
	//ak za riadok ktory sme vlozili
	if (line != 0 && data[line - 1].find('\n') == data[line - 1].npos)
		data[line - 1] += '\n';
	data.insert(data.begin() + line, insert.begin(), insert.end());
}


void insertInto(std::vector<std::string>& data, std::vector<std::string> options, bool change) {
	if (options.size() == 1) {
		writeMode(data);
	}
	else if (options.size() >= 3) {
		size_t line = data.size();
		bool range = false;
		if (change) { //ak bol zavolany pomocou change
			size_t from = 1, to = data.size();
			if (!getRanges(options[1], from, to, data.size())) {
				line = 0;
				range = false;
			}
			else {
				line = from;
				range = true;
			}
		}
		else if (options[1].find_first_not_of("0123456789") == options[1].npos) { // ci je druhy parameter cislo
			if (options[1].size() > 19) return;   //ak je cislo vacsie ako povolene
			line = std::stoull(options[1]);
			range = true;
		}

		std::vector<std::string> insert;
		if (options.size() > 3 || !range) {
			size_t oneLinerStart = options[1].length();
			oneLinerStart = (range) ? options[3].find_first_not_of("\t ", oneLinerStart) : 0;
			insert.push_back(options.back().substr(oneLinerStart, options.back().size() - oneLinerStart) + '\n');
		}
		else {
			//citaj to co chce pouzivatel pridat
			writeMode(insert);
		}
		//pridaj nove riadky do vektora
		appendToPoint(data, insert, line);
	}
}


void deleteLines(std::vector<std::string>& data, std::vector<std::string> options, bool change) {
	size_t from = 0, to = data.size();
	if (options.size() >= 3) {
		from = 1;
		if (!getRanges(options[1], from, to, data.size())) {
			if (!change) {	//ak funkcia nie je volana z c prikazu
				std::cout << "Invalid range" << std::endl;
				return;
			}
			else {	//ak je tak sa vymaze cely subor
				from = 0;
				to = data.size();
			}
		}
		else { // ak je to range
			if (from >= data.size()) {
				return;
			}
			if (to > data.size()) {
				to = data.size();
			}
		}
	}
	//ak druhy range je vacsi ako pocet riadkov
	auto last = (data.size() > to) ? (data.begin() + to) : data.end();
	data.erase(data.begin() + from, last);
}

void printFromTo(std::vector<std::string>& data,std::vector<std::string> options) {
	size_t from = 0, to = data.size();
	if (options.size() == 3) {
		from = 1;
		if (!getRanges(options[1], from, to, data.size())) {
			std::cout << "Invalid range" << std::endl;
			return;
		};
	}
	if (from >= data.size()) {
		return;
	}
	if (to > data.size()) {
		to = data.size();
	}
	std::string output = "";
	for (auto it = data.begin() + from; it != data.begin() + to; it++) {
		output+=*it;
	}
	std::cout << output;
}


bool writeChangesToFile(std::string filename,std::vector<std::string> workString) {
	std::ofstream file(filename);
	
	if (!file.is_open()) {
		std::cerr << "nepodarilo sa ulozit do suboru" << std::endl;
		return false;
	}
	for (auto& str : workString) {
		if (!(file << str)) {
			std::cerr << "ERROR pocas zapisu do suboru" << std::endl;
			return false;
		};
	}

	file.close();
	return true;
}


bool hasUnsaved(std::vector<std::string> saved, std::vector<std::string> workString) {
	if (workString.size() != saved.size()) {
		return true;
	}
	if (workString != saved) {
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
		std::cerr << "file isn't writeable" << std::endl;
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
	else {
		std::cerr << "file isn't readeable" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<std::string> saved = fileContent;

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
		if (prepinacStr.size() != 1  && prepinacStr.compare("q!")) {
			std::cout << "Unsupported command" << std::endl;
		}
		else {
			bool over = false;
			switch (prepinacStr[0]) {
			case 'w':
				if (options.size() == 1) {
					if (!writeChangesToFile(filename, fileContent)) {
						return EXIT_FAILURE;
					}
					saved = fileContent;
				}
				else { std::cout << "Unsupported command" << std::endl; }
				break;
			case 'q':
				if (options.size() == 1) {
					if (!prepinacStr.compare("q!")) { over = true; }
					if (over || !hasUnsaved(saved, fileContent)) { close = true; }
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
					deleteLines(fileContent, options,false);
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