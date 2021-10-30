#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char* argv[])
{
	if (argc != 4) {
		std::cerr << "neplatny pocet argumentov" << std::endl;
		return EXIT_FAILURE;
	}
	std::string fileName = argv[1];
	std::string pattern = argv[2];
	std::string number = argv[3];
	if (pattern.length() > 255 || pattern.empty()) {
		std::cerr << "moc dlhy string alebo prazdny: " << pattern.length() << std::endl;
		return EXIT_FAILURE;
	}

	for (auto& x : pattern) {
		if (x & 0x80) { //kontrola ci je znak vo vzore z ascii 127
			std::cerr << "Hodnota znaku je viac ako 127" << std::endl;
			return EXIT_FAILURE;
		}
	}

	if (number.length() > 10) {
		std::cerr << "moc velke cislo" << std::endl;
		return EXIT_FAILURE;
	}
	if (number.find_first_not_of("0123456789") != number.npos) {
		std::cerr << "treti parameter nie je cislo" << std::endl;
		return EXIT_FAILURE;
	}

	uint64_t num = std::stoull(number);
	if (num & 0xFFFFFFFF00000000) { // cislo ci ma viac ako je maximum pre uint32
		std::cerr << "cislo vacie ako uint32" << std::endl;
		return EXIT_FAILURE;
	}
	uint32_t numN = std::stoul(number);
	if (numN == 0) {
		std::cerr << "nemoze byt nula" << std::endl;
		return EXIT_FAILURE;
	}
	std::ifstream readFile(fileName);

	if (!readFile.is_open()) {
		std::cerr << "subor sa nepodarilo otvorit" << std::endl;
		return EXIT_FAILURE;
	}

	if (readFile.good()) {
		const size_t chunkSize = 40960;

		std::string readChunk;
		readChunk.resize(chunkSize);

		std::string workChunk;
		
		//vzdialenosti medzi chunkami
		//resetovane na nulu vzdy ked sa v chunku najde vyskyt
		size_t distanceFromLast = 0;
		size_t distanceAfterBreak = 0;


		bool lastOccDisplayed = false;
		bool firstOccurance = true;

		uint64_t numberOfBreaksBeforeLastOcc = 0;	// pocet \n po posledny vyskyt
		uint64_t numberOfBreaksAfterLastOcc = 0;	// pocet \n od posledneho vyskytu po novy najdeny vyskyt

		size_t indexOfLast = 0;

		while (readFile.read(readChunk.data(), (chunkSize - workChunk.length()))) {
			workChunk.append(readChunk.data());

			size_t index = workChunk.find(pattern);
			size_t lastInChunk = 0;		
			size_t lastBreakIndex = 0;	//pre kazdy chunk hladame 

			while (index != workChunk.npos) {
				//hladame \n od posledneho vyskytu vzoru v tom
				size_t indexBreak = workChunk.find('\n', lastInChunk);
				while (indexBreak != workChunk.npos && indexBreak < index) {
					numberOfBreaksAfterLastOcc++;
					lastBreakIndex = indexBreak + 1;
					distanceAfterBreak = 0;
					indexBreak = workChunk.find('\n', indexBreak + 1);
				}
				if (!firstOccurance && (index - lastInChunk) + distanceFromLast <= numN) {
					if (!lastOccDisplayed) {
						std::cout << numberOfBreaksBeforeLastOcc << " " << indexOfLast << std::endl;
					}

					std::cout << numberOfBreaksBeforeLastOcc + numberOfBreaksAfterLastOcc << " " << (index - lastBreakIndex) + distanceAfterBreak << std::endl;
					lastOccDisplayed = true;
				}
				else {
					lastOccDisplayed = false;
				}

				lastInChunk = index;
				indexOfLast = (index - lastBreakIndex) + distanceAfterBreak ;
				distanceFromLast = 0;
				firstOccurance = false;
				numberOfBreaksBeforeLastOcc += numberOfBreaksAfterLastOcc;
				numberOfBreaksAfterLastOcc = 0;
				index = workChunk.find(pattern, index + 1);
			}
			//dopocet vzdialenosti v chunku
			if (distanceFromLast <= numN) {
				distanceFromLast += (chunkSize - pattern.length() + 1) - lastInChunk;
			}

			//najdenie koncov riadku po poslednom najdeni vzoru
			size_t indexBreak = workChunk.find('\n', lastInChunk);
			while (indexBreak != workChunk.npos && indexBreak < (chunkSize - pattern.length() + 1)) {
				numberOfBreaksAfterLastOcc++;
				lastBreakIndex = indexBreak+1;
				distanceAfterBreak = 0;
				indexBreak = workChunk.find('\n', indexBreak + 1);
			}
			distanceAfterBreak += (chunkSize - pattern.length() + 1) - lastBreakIndex;



			if (workChunk.size() > 0) {
				workChunk.erase(0, chunkSize - pattern.length() + 1);
				if (readChunk.size() == chunkSize) { //zmeni sa velkost na prekryvanie
					readChunk.resize(chunkSize - workChunk.length());
				}
			}
		}

		//urobenie zvysnych znakov v subore
		auto zvysok = readFile.gcount();
		if (zvysok > 0) {
			readChunk.resize(zvysok);
			workChunk.append(readChunk.data());

			size_t index = workChunk.find(pattern);
			size_t lastInChunk = 0;
			size_t lastBreakIndex = 0;
			while (index != workChunk.npos) {

				size_t indexBreak = workChunk.find('\n', lastInChunk);
				while (indexBreak != workChunk.npos && indexBreak < index) {
					numberOfBreaksAfterLastOcc++;
					lastBreakIndex = indexBreak + 1;
					distanceAfterBreak = 0;
					indexBreak = workChunk.find('\n', indexBreak + 1);
				}
				if (!firstOccurance && (index - lastInChunk) + distanceFromLast <= numN) {
					if (!lastOccDisplayed) {
						std::cout << numberOfBreaksBeforeLastOcc << " " << indexOfLast << std::endl;
					}

					std::cout << numberOfBreaksBeforeLastOcc + numberOfBreaksAfterLastOcc << " " << (index - lastBreakIndex) + distanceAfterBreak << std::endl;
					lastOccDisplayed = true;
				}
				else {
					lastOccDisplayed = false;
				}

				lastInChunk = index;
				indexOfLast = (index - lastBreakIndex) + distanceAfterBreak;
				distanceFromLast = 0;
				firstOccurance = false;
				numberOfBreaksBeforeLastOcc += numberOfBreaksAfterLastOcc;
				numberOfBreaksAfterLastOcc = 0;
				index = workChunk.find(pattern, index + 1);
			}

		}

		if (!readFile.eof()) {
			std::cerr << "subor nebol precitany spravne" << std::endl;
			return EXIT_FAILURE;
		}
		readFile.close();
		return EXIT_SUCCESS;
	}

	std::cerr << "subor nebol pripraveny na citanie" << std::endl;
	return EXIT_FAILURE;
}