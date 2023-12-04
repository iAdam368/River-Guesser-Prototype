#include "Game.h"
#include <iostream>
#include <sstream>
#include <list>
#include <iomanip>
#include <thread>
#include <limits>

#ifdef _WIN32
#include <Windows.h>
void setConsoleVTMode() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hConsole, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hConsole, dwMode);
}
#endif

std::list<std::string> split(std::string s) {
	std::istringstream is(s);
	std::string field;
	std::list<std::string> result;
	while (std::getline(is, field, ',')) {
		result.push_back(field);
	}
	return result;
}

int main()
{
	//create game objects
	const std::vector<std::string> files = { "Europe.txt","Asia.txt","Africa.txt","South America.txt" };
	Rivers r(files);
	Game g(r);
	//Enable ANSI escape sequences for Windows cmd
#ifdef _WIN32
	setConsoleVTMode();
#endif
	//Introductions...
	std::cout << "Welcome to the River game. In each round, you'll be presented with two rivers and should enter:" << std::endl << std::endl;
	std::cout << "\t\033[1ms\033[0m\tif they're in the same continent" << std::endl << "\td\tif they're in different continents " << std::endl << "\tq\tto quit." << std::endl;
#ifndef ASSIGNMENT_2
	std::cout << std::endl << "Press 'enter' to get started: ";
	std::cin.get();
#else
	std::cout << std::endl << "There are 3 modes: \n\n\t1\tequal probability of continents \n\t2\tequal probability of rivers \n\t3\t50% probability of consecutive rivers being in the same continent ";
	std::cout << std::endl << "\nEnter your chosen mode (1,2 or 3) to get started: ";
	int mode;
	std::cin >> mode;
	r.setMode(mode);
#endif
	std::cout << "\033[2J\033[H\033[0;90m(enter s, d or q  - [s]ame [d]ifferent or [q]uit)\n\n\n\033[0m";
	//play until playRound returns false (this only happen when the user enters 'q')
	while (g.playRound(std::cout, std::cin)) {
		std::cout << "\n\n\n\033[0;90m(hit enter for next question...)\033[0m";
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		std::cin.get();
		std::cout << "\033[2J\033[H\033[0;90m(enter s, d or q  - [s]ame [d]ifferent or [q]uit)\n\n\n\033[0m";
	}

	//Say thanks and display scores
	std::cout << "\033[2J\033[H\n\n\n" << std::setw(11) << "" << "Thanks for playing. You scored " << g.getScore() << " / " << g.getTotal() << std::endl;
#ifdef ASSIGNMENT_2
	std::cout << "\n" << std::setw(18) << "" << "Fastest correct answers\n";
	std::cout << std::setw(10) << "" << "\033[4m\033[100;30m" << std::left << std::setw(16) << "River 1" << std::setw(16) << "River 2" << std::setw(16) << "Time(ms)\033[0m" << "\n";
	for (auto i : g.getFastest()) {
		std::cout << std::setw(10) << "" << "\033[0;90m";
		for (auto j : split(i))
			std::cout << std::left << std::setw(16) << j;
		std::cout << "\n\033[0m";
	}
#endif
	std::cout << "\n\n";
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
	std::cin.get();
	return 0;
}