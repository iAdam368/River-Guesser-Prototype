#pragma once
#include <string>
#include <vector>
#include <map>

class Rivers
{
	// BST dictionary for storing rivers
	std::map<std::string, std::string> rivers;

public:
	// Time complexity: O(n^2)
	// This method uses a for loop and a while loop which both have a time complexity of O(n). 
	// The while loop is nested within a for loop which increases the time complexity to O(n^2). 
	Rivers(const std::vector<std::string>& filenames);

	// Time complexity: O(n)
	// A for loop was used to copy over the river names (keys) from the BST dictionary into a vector of strings. 
	// The time complexity is proportional to the amount of data being transferred.
	// Therefore, the time complexity is O(n). 
	std::string getRandomRiver();

	// Time complexity: O(log n)
	// No 'for' or 'while' loops are used and instead, the data is accessed directly using the BST's key, so efficiency is improved.  
	// The time complexity is O(LogN) (logarithmic), since the count() method uses a searching algorithm that has a time complexity of O(log n). 

	bool sameContinent(std::string river1, std::string river2);

	// Time complexity: O(log n)
	// No 'for' or 'while' loops are used and instead, the data is accessed directly using the BST's key, so efficiency is improved.  
	// The time complexity is O(log n) (logarithmic), since the count() method uses a searching algorithm that has a time complexity of O(log n). 
	std::string getContinent(std::string river);



	//No need to implement this until assignment 2
	void setMode(int mode) {}
};