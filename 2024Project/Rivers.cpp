#include "Rivers.h"
#include <iostream>
#include <fstream>



Rivers::Rivers(const std::vector<std::string>& fileNames) 
{
	// Variable initialisation 
	std::random_device rd;
	gen = std::mt19937_64(rd());
	activeMode = 2; // Setting mode 2 as the default mode 
	isMode3FirstGeneration = true;
	riversInCurrentContinent = 0;

	std::mutex mutex;
	std::vector<std::thread> threads;
	for (const auto& fileName : fileNames) {
		// Each file will have it's own thread. 
		// Thread object's first parameter is the function, then object's reference, and then the function parameters. 
		// Code adapted from GeeksForGeeks, 2023.
		std::thread threadObject(&Rivers::parallelFileRead, this, fileName, std::ref(mutex));
		threads.push_back(std::move(threadObject));
		// End of adapted code. 
	}

	// Waiting on each thread to finish reading the files 
	for (std::thread& thread : threads) {
		thread.join();
	}
}



void Rivers::parallelFileRead(std::string fileName, std::mutex& mutex) 
{
	std::string fileLine;
	std::fstream textFile(fileName);

	if (textFile.is_open()) {
		std::string continent = fileName.substr(0, fileName.size() - 4);

		while (std::getline(textFile, fileLine)) {
			// Lock mutex whilst reading the file (data race protection), so only one thread accesses a single file
			mutex.lock();
			rivers.insert({ fileLine, continent });
			continentsMap[continent].push_back(fileLine);
			// Unlock the mutex after file has been read 
			mutex.unlock();
		}
		textFile.close();
	}
}



std::string Rivers::getRandomRiver() 
{
	std::string randomRiver;

	switch (activeMode) {
	case 1:
		randomRiver = getMode1River();
		break;
	case 2:
		randomRiver = getMode2River();
		break;
	case 3:
		randomRiver = getMode3River();
		break;
	default:
		randomRiver = getMode2River();
	}

	return randomRiver;
}



std::string Rivers::getMode1River() 
{
	double continentChance = 1.0 / continentsMap.size();
	double riverChance;
	std::vector<double> probabilities;

	// For loop iterator to add probabilities for all rivers 
	for (auto it = rivers.begin(); it != rivers.end(); ++it) {
		riversInCurrentContinent = continentsMap[it->second].size();
		riverChance = continentChance / riversInCurrentContinent;
		probabilities.push_back(riverChance);
	}

	std::string randomRiver = generateRiver(probabilities);
	randomRiver = lastSixRepeatCheck(randomRiver, probabilities);

	return randomRiver;
}



std::string Rivers::getMode2River() 
{
	// Populating a vector with probabilites of each river having an equal chance of being picked
	std::vector<double> probabilities;
	for (int i = 0; i < rivers.size(); ++i) {
		probabilities.push_back(1.0 / rivers.size());
	}

	std::string randomRiver = generateRiver(probabilities);
	randomRiver = lastSixRepeatCheck(randomRiver, probabilities);

	return randomRiver; 
}



std::string Rivers::getMode3River() 
{
	// Finding the last picked continent using the last picked river
	auto itr = rivers.find(lastPickedRiver);
	if (itr != rivers.end()) {
		lastPickedContinent = itr->second;
	}

	// For loop iterator to add probabilities for each river 
	std::vector<double> probabilities;
	double continentChance;
	double riverChance;
	for (auto it = rivers.begin(); it != rivers.end(); ++it) {
		riversInCurrentContinent = continentsMap[it->second].size();
		if (isMode3FirstGeneration) {
			// First time generating river in this mode (3), each continent has equal chance of being generated 
			continentChance = 1.0 / continentsMap.size();
			riverChance = continentChance / riversInCurrentContinent;
		}
		else if (lastPickedContinent == it->second) {
			// Found continent that should have 50% chance of being picked 
			riverChance = 0.5 / riversInCurrentContinent;
		}
		else {
			// Remaining continents that will have an equal chance of being picked (remaining 50%)
			continentChance = 0.5 / (continentsMap.size() - 1);
			riverChance = continentChance / riversInCurrentContinent;
		}
		probabilities.push_back(riverChance);
	}

	std::string randomRiver = generateRiver(probabilities);
	randomRiver = lastSixRepeatCheck(randomRiver, probabilities);

	lastPickedRiver = randomRiver;
	isMode3FirstGeneration = false;
	return randomRiver;
}



std::string Rivers::lastSixRepeatCheck(std::string randomRiver, std::vector<double> probabilitiesVector)
{
	if (lastSixRivers.size() > 0) {
		while (true) {
			auto it = std::find(lastSixRivers.begin(), lastSixRivers.end(), randomRiver);
			if (it != lastSixRivers.end()) {
				// randomRiver found in lastSixRivers deque, so it is a duplicate -> generate a new one
				randomRiver = generateRiver(probabilitiesVector);
				continue;
			}
			break;
		}
	}

	// Removing 'oldest' river from the deque if it contains six rivers
	if (lastSixRivers.size() == 6) {
		lastSixRivers.pop_front();
	}
	lastSixRivers.push_back(randomRiver);
	return randomRiver;
}



// Method to return a random river, given the probability as a parameter (using discrete distribution)
std::string Rivers::generateRiver(std::vector<double> probabilitiesVector) 
{
	std::discrete_distribution<> dist({ probabilitiesVector.begin(), probabilitiesVector.end() });
	int randomRiverIndex = dist(gen);
	auto it = std::next(rivers.begin(), randomRiverIndex);
	return it->first;
}



bool Rivers::sameContinent(std::string r1, std::string r2) 
{
	// Checking if rivers are valid
	std::string r1Continent = getContinent(r1);
	std::string r2Continent = getContinent(r2);
	if (r1Continent == "" || r2Continent == "") {
		return false;
	}

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
	return rivers[river];
}
