#pragma once
#include "Rivers.h"
#include <ostream>
#include <istream>
#include <chrono>
#define ASSIGNMENT_2
#define RUN_PAUSE_TEST



class Game 
{
	Rivers& r;
	int score = 0;
	int total = 0;
	std::string userInputString;
	char userInput = 'x';
	bool isCorrectAnswer = false;
	std::vector<std::pair<std::string, int>> correctlyAnsweredQuestions;

public:
	Game(Rivers& rivers) : r(rivers) { }
	int getScore() { return score; }
	int getTotal() { return total; }
	int increaseScore() { return ++score; }
	int increaseTotal() { return ++total; }
	void reset() { score = 0; total = 0; }

	bool playRound(std::ostream& out, std::istream& in);
	int getUserInputTime(std::ostream& out, std::istream& in);
	char checkUserInput(std::string userString);
	void checkTimeLimit(int timeTaken, std::ostream& out, std::istream& in);
	void processUserInput(char userInput, int timeTaken, std::string river1, std::string river2);
	std::vector<std::string> getFastest();
	void sortUserAnswers();
};
