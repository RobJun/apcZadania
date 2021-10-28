
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

bool validate(std::vector<std::vector<int>>& sudoku) {
	//validating coll and rows
	for (size_t i = 0; i < 9; i++) {
		for (size_t j = 0; j < 9; j++) {
			for (size_t k = j + 1; k < 9; k++) {
				if ((sudoku[j][i] == sudoku[k][i] && sudoku[j][i] != 0) //row
					|| (sudoku[i][j] == sudoku[i][k] && sudoku[i][j] != 0)) { // coll
					return false;
				}
			}
		}
	}
	//validating squares
	std::vector<int> square;
	for (size_t i = 0; i < 9; i++) {
		size_t coll = i % 3;
		size_t row = i / 3;
		for (size_t j = 0; j < 3; j++) {
			for (size_t k = 0; k < 3; k++) {
				if (sudoku[row * 3 + j][coll * 3 + k] != 0) {
					//kontrola ci cislo uz je vo stvorci
					for (auto el : square) {
						if (el == sudoku[row * 3 + j][coll * 3 + k]) {
							return false;
						}
					}
					square.push_back(sudoku[row * 3 + j][coll * 3 + k]);
				}
			}
		}
		//vycisti stvorec
		square.clear();

	}

	return true;
}

//skontroluje ci dane cislo moze byt vlozene do chlievika
bool canPlace(std::vector<std::vector<int>>& sudoku, size_t row, size_t cell, int num) {
	//kontrola riadku a stlpca
	for (size_t i = 0; i < 9; i++) {
		if (sudoku[row][i] == num || sudoku[i][cell] == num) {
			return false;
		}
	}

	size_t startRow = row - row % 3;
	size_t	startCol = cell - cell % 3;

	//kontrola stvorca
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (sudoku[i + startRow][j + startCol] == num) {
				return false;
			}
		}
	}

	return true;
}

// najdenie prazdeneho miesta v sudoku
bool findEmptySpace(std::vector<std::vector<int>>& sudoku, size_t& row, size_t& cell) {
	for (size_t i = 0; i < 9; i++) {
		for (size_t j = 0; j < 9; j++) {
			if (sudoku[i][j] == 0) {	//naslo sa prazdne miesto
				row = i;
				cell = j;
				return true;
			}
		}
	}
	return false; //nenaslo sa prazdne miesto
}


bool solveSudoku(std::vector<std::vector<int>>& sudoku) {
	size_t row = 0;
	size_t cell = 0;
	//najde prazdne miesto
	if (!findEmptySpace(sudoku, row, cell)) {
		return true; //ak sa nenajde prazdne miesto tak sudoku bolo cele doplnene a teda bolo vyriesene
	}

	//vyskusa kazde cislo 
	for (int triedNumber = 1; triedNumber < 10; triedNumber++) {
		if (canPlace(sudoku, row, cell, triedNumber)) { //skontroluje ci cislo mozeme vlozit do chlievika
			sudoku[row][cell] = triedNumber;
			if (solveSudoku(sudoku)) {	//pokracujeme v doplnani cisel
				return true;
			}
			sudoku[row][cell] = 0;
		}
	}
	return false;
}

//kontrola spravnosti vstupu
bool checkInput(std::string line) {
	if (line.length() != 81) { 
		return false;
	}
	for (auto it = line.begin(); it != line.end(); it++) {
		if (!std::isdigit(*it) && *it != '.') {
			return false;
		}
	}
	return true;
}


bool printSudoku(std::ostream& output, std::string line) {
	if (!(output << line)) {
		std::cerr << "Error while writing file.\n";
		return false;
	};
	return true;
}

std::vector<std::vector<int>> formatSudoku(std::string line) {
	std::vector<std::vector<int>> result;
	result.resize(9);
	for (size_t row = 0; row < 9; row++) {
		for (size_t i = 0; i < 9; i++) {
			if (line[row * 9 + i] == '.') {
				result[row].push_back(0);
			}
			else if (std::isdigit(line[row * 9 + i])) {
				result[row].push_back(line[row * 9 + i] - '0');
			}
		}
	}
	return result;
}

int main(int argc, char* argv[])
{
	//kontrola ci pocet argumentov je neparny a mensi ako 5
	if (!(argc & 0x1) || argc > 5) {
		std::cerr << "invalid number of arguments" << std::endl;
		return EXIT_FAILURE;
	}

	std::string outputFileName;
	std::string inputFileName;

	if (argc > 1) {
		for (int flagNum = 1; flagNum < argc; flagNum += 2) {
			std::string flag(argv[flagNum]);
			if (flag != "-i" && flag != "-o") {
				std::cerr << "invalid flag" << std::endl;
				return EXIT_FAILURE;
			}

			if (flag == "-i") {
				if (inputFileName.empty()) {
					std::string flagFile(argv[flagNum + 1]);
					if (flagFile == "-o" || flagFile == "-i") {
						std::cerr << "after flag must be a file name" << std::endl;
						return EXIT_FAILURE;
					}
					inputFileName = argv[flagNum + 1];
				}
				else {
					std::cerr << "two -i arguments" << std::endl;
					return EXIT_FAILURE;
				}
			}

			if (flag == "-o") {
				if (outputFileName.empty()) {
					std::string flagFile(argv[flagNum + 1]);
					if (flagFile == "-i" || flagFile == "-o") {
						std::cerr << "after flag must be a file name" << std::endl;
						return EXIT_FAILURE;
					}
					outputFileName = argv[flagNum + 1];
				}
				else {
					std::cerr << "two -o arguments" << std::endl;
					return EXIT_FAILURE;
				}
			}
		}
	}

	if (!inputFileName.empty() && !outputFileName.empty() && inputFileName == outputFileName) {
		std::cerr << "same input file as output" << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream inputFile(inputFileName);
	if (!inputFileName.empty() && !inputFile.is_open()) {
		std::cerr << "invalid file -- i" << std::endl;
		return EXIT_FAILURE;
	}
	std::ofstream  outputFile(outputFileName);
	if (!outputFileName.empty() && !outputFile.is_open()) {
		std::cerr << "invalid file -- o" << std::endl;
		return EXIT_FAILURE;
	}

	std::string line;
	std::string outputLine = "";
	while (std::getline((!inputFileName.empty()) ? inputFile : std::cin, line)) {
		//ukoncenie citania na standardnom vstupe ak nacitany riadok je prazdny
		//skontroluje sa ci nacitany riadok obsahuje 81 znakov a vsetky znaky su z 0-9 a bodky
		if (checkInput(line)) {
			auto sudoku = formatSudoku(line);
			//validuje sudoku a zaène hlada riešenie
			if (validate(sudoku) && solveSudoku(sudoku)) {
				//vytvori output string
				for (auto row : sudoku) {
					for (auto cell : row) {
						outputLine += std::to_string(cell);
					}
				}
			}
			outputLine += "\n";
			//vypísanie sudoka
			if (!printSudoku(!outputFileName.empty() ? outputFile : std::cout, outputLine)) {
				return EXIT_FAILURE;
			}
			outputLine.clear();
		}
		else {
			std::cerr << "bad sudoku" << std::endl;
			return EXIT_FAILURE;
		}
	}

	if (!inputFileName.empty()) {
		if (!inputFile.eof()) {
			std::cerr << "file wasn't read properly" << std::endl;
			return EXIT_FAILURE;
		}
		inputFile.close();
	}

	if (!outputFileName.empty()) {
		outputFile.close();
	}

	return EXIT_SUCCESS;
}