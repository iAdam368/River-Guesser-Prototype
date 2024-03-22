#pragma once
#include "Rivers.h"
#include <ostream>
#include <istream>
//#define ASSIGNMENT_2				//uncomment to switch to assignment 2
//#define RUN_PAUSE_TEST  			//uncomment to run pause test (for assignment 2 only)


class Game {
	Rivers r;
	int score = 0;
	int total = 0;
	std::string userInputString;
	char userInput;
	bool isCorrectAnswer = false;


public:
	Game(Rivers& rivers) : r(rivers) { }
	int getScore() { return score; }
	int getTotal() { return total; }

	void reset() {
		score = 0;
		total = 0;
	}

	bool playRound(std::ostream& out, std::istream& in);


	//No need to implement this until assignment 2 
	std::vector<std::string> getFastest() { return std::vector<std::string>(); }
};