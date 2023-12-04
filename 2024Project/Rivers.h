#pragma once
#include <string>
#include <vector>


class Rivers
{
	//Insert data structure(s) here...
	
public:
	//Time complexity:
	Rivers(const std::vector<std::string>& filenames);
	//Time complexity: 
	std::string getRandomRiver();
	//Time complexity: 
	bool sameContinent(std::string river1, std::string river2);
	//Time complexity:
	std::string getContinent(std::string river);

	//No need to implement this until assignment 2
	void setMode(int mode) {}
};