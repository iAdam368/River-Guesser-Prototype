#pragma once
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <random>
#include <thread>
#include <mutex>



class Rivers
{
	std::map<std::string, std::string> rivers; 
	std::map<std::string, std::vector<std::string>> continentsMap; 
	std::deque<std::string> lastSixRivers;
	std::mt19937_64 gen;
	std::string lastPickedRiver;
	std::string lastPickedContinent;
	int activeMode;
	int riversInCurrentContinent;
	bool isMode3FirstGeneration;
	
	public:
		// Time complexity: O(n log n)
		Rivers(const std::vector<std::string>& fileNames);

		void parallelFileRead(std::string fileNames, std::mutex& mutex);

		// Time complexity: O(n)
		std::string getRandomRiver();

		// Time complexity: O(log n)
		bool sameContinent(std::string river1, std::string river2);

		// Time complexity: O(log n)
		std::string getContinent(std::string river);

		void setMode(int mode) { activeMode = mode; }

		std::string generateRiver(std::vector<double> probabilitiesVector);
		std::string lastSixRepeatCheck(std::string river, std::vector<double> probabilitiesVector);
		std::string getMode1River();
		std::string getMode2River();
		std::string getMode3River();
};
