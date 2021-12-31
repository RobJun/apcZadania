
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>


bool triedenie(const std::pair<std::string, std::string>& a, const std::pair< std::string, std::string>& b) {
	return (a.second < b.second);
}


bool dostanCisla(std::string& line, std::vector<uint32_t>& result) {
	//std::vector<uint32_t> result;
	if (line.find_first_not_of("0123456789,") != line.npos)
		return false;
	if (line[0] < '0' or line[0] > '9')
		return false;
	size_t lineStart = 0;
	size_t comma = line.find(",");
	while (comma != line.npos) {

		result.push_back(std::stoul(line.substr(lineStart, comma - lineStart)));

		lineStart = comma + 1;
		comma = line.find(",",lineStart);
	}

	result.push_back(std::stoul(line.substr(lineStart, line.length() - lineStart)));

	return true;
}


std::vector<std::string> zorad(std::vector<std::string>& lines, std::vector<uint32_t>& hodnoty) {
	std::vector < std::pair<std::string, std::string>> riadky;

	size_t max = 0;
	for (auto h : hodnoty) {
		if (h > max) {
			max = h;
		}
	}
	for (auto line : lines) {
		std::string formLine;

		formLine.resize(max + (size_t)1);
		for (auto hod : hodnoty) {
			if (line.size() < hod) {
				formLine[hod] = '\0';
			}
			else {
				formLine[hod] = line[hod];

			}
		}
		std::string fin = "";
		for (size_t j = 0; j < formLine.size(); j++) {
			if (formLine[j] != '\0') {
				fin.push_back(formLine[j]);
			}
		}
		riadky.push_back({ line,fin });
	}

	std::sort(riadky.begin(), riadky.end(), triedenie);

	std::vector<std::string>  result;
	for (auto& r : riadky) {
		result.push_back(r.first);
	}



	return result;
}

int main(int argc, char* argv[]){
	if (argc != 4) {
		std::cerr << "ERROR" << std::endl;
		return EXIT_FAILURE;
	}

	std::string hodnoty = argv[3];

	std::vector<uint32_t> hodnot;
	if (!dostanCisla(hodnoty, hodnot)) {
		std::cerr << "neplatne hodnoty" << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream vstup(argv[1]);
	std::ofstream vystup(argv[2]);

	if (!vstup.is_open()) {
		std::cerr << "subor sa nepodarilo otvorit" << std::endl;
		return EXIT_FAILURE;
	}
	std::vector<std::string> subor;	
	std::string line;
	while (std::getline(vstup, line)) {
		subor.push_back(line);
	}

	if (!vstup.eof()) {
		std::cerr << "file wasn't read properly" << std::endl;
		return EXIT_FAILURE;
	}
	vstup.close();

	for (auto riadok : zorad(subor, hodnot)) {
		if (!(vystup << riadok << std::endl)) {
			std::cerr << "Error while writing to output file" << std::endl;
			return EXIT_FAILURE;
		}
	}
	vystup.close();


	return EXIT_SUCCESS;
}
