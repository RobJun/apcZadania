
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include <chrono>

#define DEBUG_RJ 1


struct DATA {
	std::string m_stav;
	char m_moveByte = 0; // x:0000 y:0000
	int64_t value = 0;
	std::vector<DATA> m_children;



	DATA() : m_stav("") {}
	DATA(std::string& stav) : m_stav(stav){}
	DATA(std::string& stav, char& move)
		: m_stav(stav),m_moveByte(move) {}


};

bool flipInDirection(std::string& stav, size_t row, size_t col, bool isBlack, std::pair<int, int>& direction, bool atleastOne) {
	if (row < 0 || row > 7 || col < 0 || col > 7 || stav[8 * row + col] == '-') return false;
	if ((isBlack && stav[row * 8 + col] == 'X') || (!isBlack && stav[row * 8 + col] == 'O')) {
		if (atleastOne) return true;
		else return false;
	}
	char beforeInsert = stav[row * 8 + col];
	stav[row * 8 + col] = (isBlack) ? 'X' : 'O';
	if (!flipInDirection(stav, row + direction.first, col + direction.second, isBlack, direction, true)) {
		stav[row * 8 + col] = beforeInsert;
		return false;
	}

	return true;
}


bool flip(std::string& stav, const size_t& row, const size_t& col, const bool& isBlack) {
	std::vector<std::pair<int, int>> directions = { {-1,-1},{-1,0}, {-1,1}, {0,-1},{0,1}, {1,-1}, {1,0}, {1,1} };
	bool result = false;
	stav[row * 8 + col] = (isBlack) ? 'X' : 'O';
	for (auto direc : directions) {
		if (flipInDirection(stav, row + direc.first, col + direc.second, isBlack, direc, false)) {
			result = true;
		}
	}
	return result;
}


std::vector<DATA> getLegitMoves(const std::string& d, bool isBlack) {
	std::vector<DATA> result;
	for (char row = 0; row < 8; row++) {
		for (char col = 0; col < 8; col++) {
			if (d[row * 8 + col] == '-') {
				std::string copy = d;
				if (flip(copy, row, col, isBlack)) {
					char move = (((row + 1) << 4) | ((col + 1) & 0xff));
					result.push_back({ copy ,move });
				}
			}
		}
	}
	return result;
}


int64_t heuristika(const std::string& d) {
	std::vector<int64_t> weights = { 4,-3,2, 2, 2, 2,-3,4,
									-3 ,-4,-1,-1,-1,-1 ,-4,-3,
									2  ,-1 ,1, 0, 0, 1, -1, 2,
									2  ,-1 ,0 , 1, 1,  0, -1,  2,
									2  ,-1 ,0 , 1, 1,  0, -1,  2,
									2  ,-1 ,1, 0, 0, 1, -1, 2,
									-3 ,-4,-1,-1,-1,-1 ,-4,-3,
									4  ,-3,2, 2, 2, 2,-3,4
								};
	std::vector<std::pair<int, int>> directions = { {-1,-1},{-1,0}, {-1,1}, {0,-1},{0,1}, {1,-1}, {1,0}, {1,1} };
	int64_t dz = 0, p = 0, f = 0, m = 0;
	int64_t blackFront = 0, whiteFront = 0;
	int64_t blackT = 0, whiteT = 0;
	for (size_t i = 0; i < 8; i++) {
		for (size_t j = 0; j <8; j++) {
			if ('X' == d[i * 8 + j]) {
				dz += weights[i];
				blackT++;
			}
			else if ('O' == d[i * 8 + j]) {
				dz -= weights[i];
				whiteT++;
			}
			if (d[i * 8 + j] != '-') {
				for (auto& dir : directions) {
					size_t x = i + dir.first;
					size_t y = j + dir.second;
					if (x >= 0 && x < 8 && y >= 0 && y < 8 && d[x*8+y] == '-') {
						if (d[i * 8 + j] == 'X')  blackFront++;
						else whiteFront++;
						break;
					}
				}
			}
		}
	}
	if (blackT > whiteT)
		p = (100 * blackT) / (blackT + whiteT);
	else if (blackT < whiteT)
		p = -(100 * whiteT) / (blackT + whiteT);
	else p = 0;

	if (blackFront > whiteFront)
		f = -(100 * blackFront) / (blackFront + whiteFront);
	else if (blackFront < whiteFront)
		f = (100 * whiteFront) / (blackFront + whiteFront);
	else f = 0;


	// Corner occupancy
	blackT = whiteT = 0;
	if (d[0] == 'X') blackT++;
	else if (d[0] == 'O') whiteT++;
	if (d[7] == 'X') blackT++;
	else if (d[7] == 'O') whiteT++;
	if (d[7*8] == 'X') blackT++;
	else if (d[7 * 8] == 'O') whiteT++;
	if (d[7 * 8+7] == 'X') blackT++;
	else if (d[7 * 8 + 7] == 'O') whiteT++;
	int64_t c = 25 * (blackT - whiteT);

	//corner closeness
	blackT = whiteT = 0;
	if (d[0] == '-') {
		if (d[8] == 'X') blackT++;
		else if (d[8] == 'O') whiteT++;
		if (d[9] == 'X') blackT++;
		else if (d[9] == 'O') whiteT++;
		if (d[1] == 'X') blackT++;
		else if (d[1] == 'O') whiteT++;
	}
	if (d[7] == '-') {
		if (d[6] == 'X') blackT++;
		else if (d[6] == 'O') whiteT++;
		if (d[8+6] == 'X') blackT++;
		else if (d[8+6] == 'O') whiteT++;
		if (d[8+7] == 'X') blackT++;
		else if (d[8+7] == 'O') whiteT++;
	}
	if (d[8*7] == '-') {
		if (d[8*7+1] == 'X') blackT++;
		else if (d[8*7+1] == 'O') whiteT++;
		if (d[8*6+1] == 'X') blackT++;
		else if (d[8*6+1] == 'O') whiteT++;
		if (d[8*6+0] == 'X') blackT++;
		else if (d[8*6+0] == 'O') whiteT++;
	}
	if (d[8*7+7] == '-') {
		if (d[8*6+7] == 'X') blackT++;
		else if (d[8 * 6 + 7] == 'O') whiteT++;
		if (d[8 * 6 + 6] == 'X') blackT++;
		else if (d[8 * 6 + 6] == 'O') whiteT++;
		if (d[8 * 7 + 6] == 'X') blackT++;
		else if (d[8 * 7 + 6] == 'O') whiteT++;
	}
	int64_t l = -12 * (blackT - whiteT);

	blackT = getLegitMoves(d,true).size();
	whiteT = getLegitMoves(d, false).size();
	if(blackT > whiteT)
		m = (100 * blackT)/(blackT + whiteT);
	else if(blackT < whiteT)
		m = -(100 * whiteT)/(blackT + whiteT);
	else m = 0;


	return 10*p + 74*f + 10*dz + 801 * c + 70*m + 380*l;
}


bool tryTime(std::chrono::time_point< std::chrono::steady_clock>& start, const uint64_t& maxTime, std::chrono::duration < double >& delta) {
	std::chrono::duration < double > deltaTime = delta = std::chrono::steady_clock::now() - start;
	return deltaTime.count() > (double)maxTime - 0.1f;
}

std::pair<bool,std::pair<int64_t,char>> minmax(DATA & d, int64_t depth,
											   int64_t alpha, int64_t beta,
											   bool isBlack, std::chrono::time_point<std::chrono::steady_clock> start,
											   uint64_t maxTime,int64_t depthToSave) {
	std::chrono::duration < double > deltaTime;
	if (tryTime(start, maxTime, deltaTime)) {
		return { true,{0,(char)0}};
	}
	if (depth == 0) {
		int64_t s = heuristika(d.m_stav);
		//std::clog << "leaf: " << s << " ";
		return { false, {s,d.m_moveByte} };
	}


	std::vector<DATA> children;
	const int maxSavedNodesDepht = 8;

	if (d.m_children.empty()) {
		children = getLegitMoves(d.m_stav,isBlack);
		if (children.empty()) {
			if (getLegitMoves(d.m_stav, !isBlack).empty()) {
				return { false, {heuristika(d.m_stav),d.m_moveByte} };
			}
			DATA c = d;
			children.push_back(c);
		}
		if (depthToSave <= maxSavedNodesDepht) {
			d.m_children = children;
		}
	}

	char retByte = 0;
	if (isBlack) {
		int64_t maxVal = INT64_MIN;
		for (DATA& child : (d.m_children.empty()) ? children : d.m_children) {
			auto ret =  minmax(child, depth - 1, alpha, beta, false, start, maxTime,depthToSave+1);
			if (ret.first) return ret;
			if (maxVal < ret.second.first) {
				maxVal = ret.second.first;
				retByte = child.m_moveByte;
			}
			alpha = std::max(alpha, ret.second.first);
			if (alpha >= beta) {
				break;
			}
		}
		if (depth == 1) {
			std::sort(d.m_children.begin(), d.m_children.end(), [](DATA a, DATA b) {return (a.value > b.value); });
		}

		d.value = maxVal;
		//std::clog << std::endl << "maximazing: " << maxVal << "[" << depth << "]" << " --- ";
		return { false,{maxVal,retByte} };
	}
	else {
		int64_t minVal = INT64_MAX;
		for (DATA& child : (d.m_children.empty()) ? children : d.m_children) {
			auto ret = minmax(child, depth - 1, alpha, beta, true, start, maxTime, depthToSave + 1);
			if (ret.first) return ret;
			if (minVal > ret.second.first) {
				minVal = ret.second.first;
				retByte = child.m_moveByte;
			}
			beta = std::min(beta, ret.second.first);
			if (alpha >= beta) {
				break;
			}
		}

		if (depth == 1) {
			std::sort(d.m_children.begin(), d.m_children.end(), [](DATA a, DATA b) {return (a.value < b.value); });
		}

		d.value = minVal;
		return { false, {minVal,retByte} };
	}
}



std::pair<int, int> countTree(DATA& d) {
	int maxDepth = 0;
	int count = 1;
	for (auto& c : d.m_children) {
		auto res = countTree(c);
		maxDepth = std::max(maxDepth, res.first);
		count += res.second;
	}
	return { maxDepth+1,count };
}



std::pair<char,uint64_t> iterativeSearch(DATA& d, bool isBlack, std::chrono::time_point<std::chrono::steady_clock> start, uint64_t maxTime,uint64_t lastDepth) {
	uint64_t depth = lastDepth;
	char bestMove = 0;
	int64_t maxScore = 0;											
	uint64_t countALL = std::count(d.m_stav.begin(),d.m_stav.end(),'-');
	while (depth != countALL+1) {
		std::chrono::duration < double > deltaTime;
		if (tryTime(start, maxTime, deltaTime)) {

#if DEBUG_RJ
			std::clog << "cas: " << deltaTime.count() << " [ " << maxTime << " ]" << std::endl;
#endif // DEBUG
			break;
		}
			


		auto searchResult = minmax(d, depth, INT64_MIN, INT64_MAX,isBlack, start, maxTime,0);
		if (!searchResult.first) {
			maxScore = searchResult.second.first;
			bestMove = searchResult.second.second;
		}
		if (searchResult.first) {
			depth--;
			break;
		}


		depth++;
	}

#if DEBUG_RJ
	auto res = countTree(d);
	std::clog << "hlbka : " << res.first << "     pocet" << res.second << std::endl;
	std::clog << "hlbka searchu :" << depth << std::endl;
	std::clog << "najlepsia hodnota:  " << maxScore << std::endl;
#endif
	return { bestMove,depth };
}

std::pair<int,std::vector<std::string>> getCommand(std::string& line) {
	std::string temp;
	std::stringstream stream(line);
	std::vector<std::string> result;
	while (stream >> temp) {
		result.push_back(temp);
		if (result.size() > 3) {
			return { 0,result };
		}
	}
	if (result.size() != 0) {
		if (result[0] == "START") {
			if (result.size() == 3) return { 1,result };
		}
		else if (result[0] == "STOP") {
			if (result.size() == 1) return { 2,result };
		}
		else if (result[0] == "MOVE") {
			if (result.size() == 2) return { 3,result };
		}
	}


	return { 0,result };
}

int main() {	
	std::string command;
	uint64_t maxCas = 0;
	bool started = false;
	bool color = false; 
	std::string pos;
	pos.resize(3);
	DATA d;
	uint64_t lastDepth = 0;



	while (std::getline(std::cin, command)) {
		auto prijatieSpravy = std::chrono::steady_clock::now();
		std::string result;
		auto com = getCommand(command);
		if (!com.first) {
			std::clog << "neplatny pocet argumentov" << std::endl;
			return EXIT_FAILURE;
		}
		if (com.first == 1) { //START <B> <time>
			if (started) {
				std::clog << "hra uz zacala" << std::endl;
				return EXIT_FAILURE;
			}
			if (com.second[2].find_first_not_of("0123456789") != std::string::npos) {
				std::clog << "neplatny cas" << std::endl;
				return EXIT_FAILURE;
			}
			if (com.second[1] != "B" && com.second[1] != "W") {
				std::clog << "neplatny hrac" << std::endl;
				return EXIT_FAILURE;
			}
			color = (com.second[1] == "B") ? true : false;
			maxCas = std::stoull(com.second[2]);
			if (maxCas == 0) {
				return EXIT_FAILURE;
				}
			result = "1";
			started = true;
		}
		else if (com.first == 2) { //STOP
			if (!started) {
				std::clog << "hra nezacala" << std::endl;
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}
		if (com.first == 3) { //MOVE ---------------------------OX------XO---------------------------
			if (!started) {
				std::clog << "hra nezacala" << std::endl;
				return EXIT_FAILURE;
			}
			std::string stav = com.second[1];
			if (stav.length() != 64 || stav.find_first_not_of("OX-") != stav.npos) {
				std::clog << "neplatny stav" << std::endl;
				return EXIT_FAILURE;
			}
			if (!d.m_stav.empty()) {
				bool found = false;
				for (auto& child : d.m_children) {
					for (DATA cc : child.m_children) {
						if (cc.m_stav == stav) {
							d = cc;
							found = true;
							if (d.m_children.empty()) {
								d.m_children = getLegitMoves(stav, color);
							}
							break;
						}
					}
					if (found) {
						break;
					}
				}
				if (!found) {
					d = { stav };
					d.m_children = getLegitMoves(stav, color);
					lastDepth = 1;
				}
			}
			else {
				d = { stav };
				d.m_children = getLegitMoves(stav, color);
				lastDepth = 1;
			}
			if (d.m_children.empty()) {
				std::clog << "ziadne pohyby" << std::endl;
				return EXIT_FAILURE;
			}
			auto res = iterativeSearch(d, color, prijatieSpravy, maxCas,lastDepth);
			//lastDepth = (res.second - 1 > 0) ? res.second - 1 : 1;
			lastDepth = 1;
			char row = (res.first &0xf0) >> 4;
			char col = res.first & 0x0f;
			pos[0] = ('A' - 1 + col);
			pos[1] = ('0' + row);
			result = pos;
		}
#if DEBUG_RJ
		std::chrono::duration < double > deltaTime = std::chrono::steady_clock::now() - prijatieSpravy;
		std::clog << "celkovy cas: " << deltaTime.count() << std::endl;
#endif
		std::cout << result << std::endl;
	}
	return EXIT_SUCCESS;
}
