#include "Game.h"

bool Game::playRound(std::ostream& out, std::istream& in) {

	// Generating two random rivers
	std::string r1 = r.getRandomRiver();
	std::string r2 = r.getRandomRiver();

	// Outputting two rivers as long as they are both different
	while (r1 == r2) {
		r2 = r.getRandomRiver();
	}
	out << r1 << std::endl;
	out << r2 << std::endl;

	// Getting the continets for the rivers
	std::string c1 = r.getContinent(r1);
	std::string c2 = r.getContinent(r2);

	// Boolean for continent check
	bool isSameContinent = r.sameContinent(r1, r2);

	// Receive user input and check if it's a character
	in >> userInputString;
	if (userInputString.length() > 1) {
		// Set to x as it's a string and then simply treated as an incorrect input
		userInput = 'x';
	}
	else {
		userInput = userInputString[0];
	}

	// Checking for the correct response and updating the score accordingly
	if (userInput == 's') {
		if (isSameContinent) {
			isCorrectAnswer = true;
			++score;
		}
		else {
			isCorrectAnswer = false;
		}
	}
	else if (userInput == 'd') {
		if (isSameContinent) {
			isCorrectAnswer = false;
		}
		else {
			isCorrectAnswer = true;
			++score;
		}
	}
	else if (userInput == 'q') {
		return false;
	}
	else {
		isCorrectAnswer = false;
	}

	// Print correct or incorrect and the actual continents of the rivers
	if (isCorrectAnswer) {
		out << "Correct: \n";
	}
	else {
		out << "Incorrect: \n";
	}
	out << "The river " << r1 << " is in " << c1 << "\n";
	out << "The river " << r2 << " is in " << c2 << "\n";

	++total;
	return true;
}