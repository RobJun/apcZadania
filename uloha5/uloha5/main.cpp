
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::pair<bool, std::string> compressLine(const std::string& line) {
	std::string result = "";
	if (line.empty()) return { true,result };
	if (line.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != line.npos) return { false, result };
	size_t indexOfFirst = 0, indexOfLast = line.find_first_not_of(line[indexOfFirst],indexOfFirst);
	while (indexOfLast != line.npos) {
		result += line[indexOfFirst] + std::to_string(indexOfLast - indexOfFirst);
		indexOfFirst = indexOfLast;
		indexOfLast = line.find_first_not_of(line[indexOfFirst], indexOfFirst);
	}
	result += line[indexOfFirst] + std::to_string(line.length() - indexOfFirst);

	return { true,result };
}

std::pair<bool, std::string> decompressLine(const std::string& line) {
	std::string result = "";
	if (line.empty()) return { true,result };
	if (line.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != line.npos) return { false, result };
	size_t index = 0, lastIndexOfAbb = line.find_first_not_of("0123456789",1);
	while (lastIndexOfAbb != line.npos) {
		if (lastIndexOfAbb == index + 1) return { false,result };
		uint64_t count = std::stoull(line.substr(index + 1, lastIndexOfAbb - (index + 1)));
		if (count == 0) return { false, result };
		if ((line[index] >= 'a' && line[index] <= 'z') || (line[index] >= 'A' && line[index] <= 'Z')) {
			for (uint64_t i = 0; i < count; i++) {
				result += line[index];
			}
			index = lastIndexOfAbb;
			lastIndexOfAbb = line.find_first_not_of("0123456789", index+1);
		}
		else {
			return { false, result };
		}

	}
	if (line.length() == index + 1) return { false,result };
	uint64_t count = std::stoull(line.substr(index + 1, line.length() - (index + 1)));
	if (count == 0) return { false, result };
	if ((line[index] >= 'a' && line[index] <= 'z') || (line[index] >= 'A' && line[index] <= 'Z')) {
		for (uint64_t i = 0; i < count; i++) {
			result += line[index];
		}
	}
	else {
		return { false, result };
	}
	return { true,result };
}


typedef std::pair<bool, std::string>(*function_t)(const std::string& line);

int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "neplatny pocet argumentov" << std::endl;
		return EXIT_FAILURE;
	}
	std::string operationName = argv[1], inFilename = argv[2], outFilename = argv[3];
	function_t operation = nullptr;

	if (operationName == "compress") {
		operation = compressLine;
	}
	else if (operationName == "decompress") {
		operation = decompressLine;
	}
	else {
		std::cerr << "neplatna operacia" << std::endl;
		return EXIT_FAILURE;
	}
	std::ifstream inFile(inFilename);
	if (!inFile.is_open()) {
		std::cerr << "nepodarilo sa otvorit vstupny subor" << std::endl;
		return EXIT_FAILURE;
	}
	std::vector<std::string> results;
	std::string line;
	int lineIndex = 1;
	while (std::getline(inFile, line)) {
		std::pair<bool, std::string> lineAfter = operation(line);
		if (!lineAfter.first) {
			std::cout << "Error on line " << lineIndex << std::endl;
			return EXIT_FAILURE;
		}
		results.push_back(lineAfter.second);
		lineIndex++;
	}

	if (!inFile.eof()) {
		std::cerr << "error pri citani suboru" << std::endl;
		return EXIT_FAILURE;
	}

	inFile.close();

	std::ofstream outFile(outFilename);
	if (!outFile.is_open()) {
		std::cerr << "nepodarilo sa otvorit vystupny subor" << std::endl;
		return EXIT_FAILURE;
	}

	for (auto row : results) {
		if (!(outFile << row << std::endl)) {
			std::cerr << "Error while writing to output file" << std::endl;
			return EXIT_FAILURE;
		}
	}
	outFile.close();

	return EXIT_SUCCESS;
}
