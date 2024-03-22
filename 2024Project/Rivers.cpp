#include "Rivers.h"
#include <iostream>
#include <fstream>
#include <random>

Rivers::Rivers(const std::vector<std::string>& filenames)
{
	std::string fileLine;

	for (const auto& filename : filenames) {
		std::ifstream textFile(filename);
		std::string continent = filename.substr(0, filename.size() - 4);

		if (textFile.fail()) {
			std::cerr << "There has been an error trying to open " << filename << std::endl;
		}
		else {
			fileLine = "";

			while (std::getline(textFile, fileLine)) {
				rivers.insert({ fileLine, continent });
			}
			textFile.close();
		}
	}
}

std::string Rivers::getRandomRiver()
{
	// Random number generator in the range between 0 and the highest river index
	// Start of adapted code from StackOverflow user named Cornstalks, (2012)
	std::random_device randomSeed;
	std::mt19937 rng(randomSeed());
	std::uniform_int_distribution<std::mt19937::result_type> range(0, rivers.size() - 1);
	int randomRiverIndex = range(rng);
	// End of adapted code

	// Iterating through the BST dictionary from the beginning until the random index
	auto i = std::next(rivers.begin(), randomRiverIndex);
	return i->first;
}

bool Rivers::sameContinent(std::string r1, std::string r2)
{
	// Checking if rivers are valid
	std::string r1Continent = getContinent(r1);
	std::string r2Continent = getContinent(r2);
	if (r1Continent == "" || r2Continent == "") {
		return false;
	}

	// Comparing the two continents 
	if (r1Continent == r2Continent) {
		return true;
	}
	return false;
}

std::string Rivers::getContinent(std::string river)
{
	// Checking if the river is valid
	if (rivers.count(river) == 0) {
		return "";
	}

	// Returning the contient for the river using an iterator
	return rivers[river];
}