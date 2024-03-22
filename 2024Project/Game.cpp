#include "Game.h"



bool Game::playRound(std::ostream& out, std::istream& in) 
{
	// Generating two random rivers
	std::string river1 = r.getRandomRiver();
	std::string river2 = r.getRandomRiver();
	out << river1 << "\n";
	out << river2 << "\n";

	int timeTaken = getUserInputTime(out, in);
	userInput = checkUserInput(userInputString);
	checkTimeLimit(timeTaken, out, in);

	processUserInput(userInput, timeTaken, river1, river2);

	if (userInput == 'q')
		return false;

	// Printing result and the actual answer
	if (isCorrectAnswer) { 
		out << "Correct: \n"; 
	}
	else { 
		out << "Incorrect: \n"; 
	}
	out << "The river " << river1 << " is in " << r.getContinent(river1) << "\n" << "The river " << river2 << " is in " << r.getContinent(river2) << "\n";

	increaseTotal();
	return true;
}



int Game::getUserInputTime(std::ostream& out, std::istream& in) 
{
	auto startTime = std::chrono::steady_clock::now();
	in >> userInputString;
	auto endTime = std::chrono::steady_clock::now();
	int timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
	return timeTaken;
}



char Game::checkUserInput(std::string userString) 
{
	if (userString.length() > 1) {
		// Set char input to 'x' as input was a string, and then simply treated as an incorrect input
		userInput = 'x';
	}
	else {
		userInput = userString[0];
	}
	return userInput;
}



void Game::checkTimeLimit(int timeTaken, std::ostream& out, std::istream& in) 
{
	// Checking whether user took more than 10 seconds (10,000 milliseconds) to enter a response
	if (timeTaken >= 10000) {
		// Set char input to 'x' as the time limit is exceeded, and then simply treat it as an incorrect input
		userInput = 'x';
		out << "Time limit exceeded!\n";
	}
}



void Game::processUserInput(char userInput, int timeTaken, std::string river1, std::string river2) {
	// Checking for the correct response and updating the score accordingly
	isCorrectAnswer = (userInput == 's' && r.sameContinent(river1, river2)) || (userInput == 'd' && !r.sameContinent(river1, river2));
	if (isCorrectAnswer) {
		correctlyAnsweredQuestions.emplace_back(river1 + "," + river2, timeTaken);
		increaseScore();
	}
}



std::vector<std::string> Game::getFastest() 
{
	sortUserAnswers();

	// Adding top five fastest answers in correctlyAnsweredQuestions to topFiveFastestAnswers vector list 
	std::vector<std::string> topFiveFastestAnswers;
	int topFiveCounter = 0;
	for (const auto& answer : correctlyAnsweredQuestions) {
		if (topFiveCounter >= 5) {
			break;
		}
		topFiveFastestAnswers.push_back(answer.first + "," + std::to_string(answer.second));
		topFiveCounter++;
	}
	return topFiveFastestAnswers;
}



// Sorting all answers in the correctlyAnsweredQuestions vector list into descending order (fastest at the top) using a lambda function 
void Game::sortUserAnswers() 
{
	// Code adapted from Sneh, 2022.
	std::sort(correctlyAnsweredQuestions.begin(), correctlyAnsweredQuestions.end(), [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });
	// End of adapted code. 
}
