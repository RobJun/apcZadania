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
	std::string vzor = argv[2];
	std::string number = argv[3];
	if (vzor.length() > 255) {
		std::cerr << "moc dlhy string" << std::endl;
		return EXIT_FAILURE;
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
	if (num & 0xFFFFFFFF00000000) {
		std::cerr << "cislo vacie ako uint32" << std::endl;
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
		const size_t chunkSize = 5;

		std::string readChunk;
		readChunk.resize(chunkSize);

		std::string workChunk;
		
		size_t distanceFromLast = 0;
		bool lastOccDisplayed = false;
		bool firstOccurance = true;

		uint32_t numberOfBreaksBeforeLastOcc = 0;
		uint32_t numberOfBreaksAfterLastOcc = 0;
		size_t distanceAfterBreak = 0;

		bool moreChunksLoaded = false;

		size_t indexOfLast = 0;

		while (readFile.read(readChunk.data(), (chunkSize - workChunk.length()))) {
			workChunk.append(readChunk.data());

			size_t index = workChunk.find(vzor);
			size_t lastInChunk = 0;		
			size_t nextBreakIndex = 0;

			while (index != workChunk.npos) {

				size_t indexBreak = workChunk.find('\n', lastInChunk);
				while (indexBreak != workChunk.npos && indexBreak < index) {
					if (!moreChunksLoaded || indexBreak >= vzor.length()-1) {
						numberOfBreaksAfterLastOcc++;
						nextBreakIndex = indexBreak + 1;
						distanceAfterBreak = 0;
					}
					indexBreak = workChunk.find('\n', indexBreak + 1);
				}
				if (!firstOccurance && (index - lastInChunk) + distanceFromLast <= num_N) {
					if (!lastOccDisplayed) {
						std::cout << numberOfBreaksBeforeLastOcc << " " << indexOfLast << std::endl;
					}

					std::cout << numberOfBreaksBeforeLastOcc + numberOfBreaksAfterLastOcc << " " << (index - nextBreakIndex) + distanceAfterBreak << std::endl;
					lastOccDisplayed = true;
				}
				else {
					lastOccDisplayed = false;
				}

				lastInChunk = index;
				indexOfLast = (index - nextBreakIndex) + distanceAfterBreak ;
				distanceFromLast = 0;
				firstOccurance = false;
				numberOfBreaksBeforeLastOcc += numberOfBreaksAfterLastOcc;
				numberOfBreaksAfterLastOcc = 0;
				index = workChunk.find(vzor, index + 1);
			}
			if (distanceFromLast <= num_N) {
				distanceFromLast += (chunkSize - vzor.length() + 1) - lastInChunk;
			}


			size_t indexBreak = workChunk.find('\n', lastInChunk);
			while (indexBreak != workChunk.npos) {
				if (indexBreak >= vzor.length()-1) {
					numberOfBreaksAfterLastOcc++;
					nextBreakIndex = indexBreak+1;
					distanceAfterBreak = 0;
				}
				indexBreak = workChunk.find('\n', indexBreak + 1);
			}
			distanceAfterBreak += (chunkSize - vzor.length() + 1) - nextBreakIndex;



			if (workChunk.size() > 0) {
				workChunk.erase(0, chunkSize - vzor.length() + 1);
				readChunk.resize(chunkSize - workChunk.length());
			}

			moreChunksLoaded = true;
		}

		auto zvysok = readFile.gcount();
		if (zvysok > 0) {
			readChunk.resize(zvysok);
			workChunk.append(readChunk.data());

			size_t index = workChunk.find(vzor);
			size_t lastInChunk = 0;
			size_t nextBreakIndex = 0;
			while (index != workChunk.npos) {

				size_t indexBreak = workChunk.find('\n', lastInChunk);
				while (indexBreak != workChunk.npos && indexBreak < index) {
					if (!moreChunksLoaded || indexBreak >= vzor.length() - 1) {
						numberOfBreaksAfterLastOcc++;
						nextBreakIndex = indexBreak + 1;
						distanceAfterBreak = 0;
					}
					indexBreak = workChunk.find('\n', indexBreak + 1);
				}
				if (!firstOccurance && (index - lastInChunk) + distanceFromLast <= num_N) {
					if (!lastOccDisplayed) {
						std::cout << numberOfBreaksBeforeLastOcc << " " << indexOfLast << std::endl;
					}

					std::cout << numberOfBreaksBeforeLastOcc + numberOfBreaksAfterLastOcc << " " << (index - nextBreakIndex) + distanceAfterBreak << std::endl;
					lastOccDisplayed = true;
				}
				else {
					lastOccDisplayed = false;
				}

				lastInChunk = index;
				indexOfLast = (index - nextBreakIndex) + distanceAfterBreak;
				distanceFromLast = 0;
				firstOccurance = false;
				numberOfBreaksBeforeLastOcc += numberOfBreaksAfterLastOcc;
				numberOfBreaksAfterLastOcc = 0;
				index = workChunk.find(vzor, index + 1);
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