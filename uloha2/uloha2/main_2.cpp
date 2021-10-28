#include <iostream>
#include <string>
#include <fstream>
#include <vector>


int main(int argc, char* argv[])
{
	if (argc != 4) {
		std::cerr << "neplatny pocet argumentov" << std::endl;
		return EXIT_FAILURE;
	}
	std::string fileName = argv[1];
	std::string vzor = argv[2];
	std::string number = argv[3];
	if (vzor.length() > 255) {
		std::cerr << "moc dlhy string" << std::endl;
		return EXIT_FAILURE;
	}
	if (number.find_first_not_of("0123456789") != number.npos) {
		std::cerr << "treti parameter nie je cislo" << std::endl;
		return EXIT_FAILURE;
	}


	uint32_t num_N = std::stoul(number);
	if (num_N == 0) {
		std::cerr << "nemoze byt nula" << std::endl;
		return EXIT_FAILURE;
	}
	std::ifstream readFile(fileName);

	if (!readFile.is_open()) {
		std::cerr << "subor sa nepodarilo otvorit" << std::endl;
		return EXIT_FAILURE;
	}


	if (readFile.good()) {
		const size_t chunkSize = 100;

		std::string readChunk;
		readChunk.resize(chunkSize);

		std::string workChunk;

		//nahradit iba premenami
		//std::vector<size_t> lineBreaks;
		uint32_t numberOfBreaksBeforeLastOcc = 0;
		uint32_t numberOfBreaksAfterLastOcc = 0;

		size_t lastBreakIndex = 0;
		size_t lastOccBreakIndex = 0;
		size_t accIndex = 0;
		size_t lastOccIndex = 0;

		bool lastOccDisplayed = false;
		bool firstOccurance = true;
		while (readFile.read(readChunk.data(), (chunkSize - workChunk.length()))) {
			workChunk.append(readChunk.data());

			size_t index = workChunk.find(vzor);
			while (index != workChunk.npos) {
				size_t fileIndex = accIndex + index;

				size_t indexBreak = workChunk.find('\n');
				while (indexBreak != workChunk.npos && accIndex + indexBreak < fileIndex) {
					if (accIndex + indexBreak > lastBreakIndex) {
						lastBreakIndex = accIndex + indexBreak;
						if (lastBreakIndex > lastOccIndex) {
							numberOfBreaksAfterLastOcc++;
						}
					}
					indexBreak = workChunk.find('\n', indexBreak + 1);
				}
				if (accIndex == 0 or fileIndex > lastOccIndex) {
					if (!firstOccurance && fileIndex - lastOccIndex <= num_N) {
						if (!lastOccDisplayed) {
							std::cout << numberOfBreaksBeforeLastOcc << " " << lastOccIndex << std::endl;
						}
						std::cout << numberOfBreaksBeforeLastOcc + numberOfBreaksAfterLastOcc << " " << fileIndex << std::endl;
						lastOccDisplayed = true;
					}
					else {
						lastOccDisplayed = false;
					}
					numberOfBreaksBeforeLastOcc += numberOfBreaksAfterLastOcc;
					numberOfBreaksAfterLastOcc = 0;
					lastOccIndex = fileIndex;
					lastOccBreakIndex = lastBreakIndex;
				}
				firstOccurance = false;
				index = workChunk.find(vzor, index + 1);

			}

			size_t indexBreak = workChunk.find('\n');
			while (indexBreak != workChunk.npos) {
				if (accIndex + indexBreak > lastBreakIndex) {
					lastBreakIndex = accIndex + indexBreak;
					if (lastBreakIndex > lastOccIndex) {
						numberOfBreaksAfterLastOcc++;
					}
				}
				indexBreak = workChunk.find('\n', indexBreak + 1);
			}



			if (workChunk.size() > 0) {
				workChunk.erase(0, chunkSize - vzor.length() + 1);
				accIndex += chunkSize - vzor.length() + 1;
				readChunk.resize(chunkSize - workChunk.length());
			}

		}

		auto zvysok = readFile.gcount();
		if (zvysok > 0) {
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
