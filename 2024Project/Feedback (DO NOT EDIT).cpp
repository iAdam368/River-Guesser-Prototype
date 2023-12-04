#include "Rivers.h"
#include "Game.h"
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <cctype>
#include <functional>
#include <thread>
const std::vector<std::string> files = { "Europe.txt","Asia.txt","Africa.txt","South America.txt" };
const std::unordered_map<std::string, std::vector<std::string> > riverData = { {"Europe", {"Rhone","Rhine","Danube","Loire","Volga"}} ,
																				{"Asia" , {"Ob", "Kolyma", "Yangtze", "Lena", "Ganges"}},
																				{"Africa", {"Congo","Niger","Nile", "Limpopo", "Zambezi"}},
																				{"South America", {"Amazon","Orinoco","Paraguay","Rio Grande"}}
};


template<class Suite>
struct test {
	bool passed;
	bool executed;
	std::string fail;
	std::function<bool(Suite&)> fnc;
	test(std::function<bool(Suite&)> fn, std::string failMsg) : fnc(fn), fail(failMsg), passed(false), executed(false) {}
	void operator () (Suite& tc) {
		if (!executed)
			passed = fnc(tc);
		executed = true;
	}
	test() :passed(false), executed(false), fail("") {}
};

template<class Inherited>
class TestSuite {
protected:
	std::unordered_map<std::string, test<Inherited>> tests;
public:
	TestSuite() {}
	virtual void operator () () { for (auto i = tests.begin(); i != tests.end(); ++i) i->second(*(static_cast<Inherited*>(this))); }
	virtual void results(std::ostream& o);
};

template<class Inherited>
void TestSuite<Inherited>::results(std::ostream& o) {
	int count = 0;
	o << "\033[0;31m";
	for (auto t : tests) { 
		if (!t.second.executed)
			o << "\n" << t.first << ": test not executed";
		if (!t.second.passed && t.second.executed) 
			o << "\n\t" << t.second.fail; 
		if (t.second.passed)
			count++;
	}
	if (count == tests.size())
		o << "\033[0;32m\n\tNo Issues\t\t";
	o << "\033[0m";
}


class RiverTests : public TestSuite<RiverTests>{
	Rivers r;
	std::unordered_map<std::string, int> histogram;
	bool checkRand();
	bool checkBogusRiver();
	bool checkBogusPair();
	bool checkAllRivers();
	bool checkSame();
	bool checkFiles();
	bool checkContinents();
	bool checkPermutation();
	bool checkHardcoded();
	const int samples = 5000;
public:
	RiverTests();
	int score();
	virtual void results(std::ostream& o) { o << "\nRivers class - issue list: "; TestSuite<RiverTests>::results(o); }
	bool noHardcoded() { return tests["Hardcoded"].passed; }
};

struct Renamer {
	Renamer() {
		for (auto i : files)
			std::filesystem::rename(i, i + ".bak");
	}
	~Renamer() {
		for (auto i : files)
			std::filesystem::rename(i + ".bak", i);
	}
};

RiverTests::RiverTests() :r(files) {
	tests["Random"] = test<RiverTests>(&RiverTests::checkRand, "Random data does not appear to be very random");
	tests["All Rivers"] = test<RiverTests>(&RiverTests::checkAllRivers, "getRandomRivers() does not produce the full range of values");
	tests["Files"] = test<RiverTests>(&RiverTests::checkFiles, "Files read outside of the constructor (or not read successfully at all)");
	tests["Same"] = test<RiverTests>(&RiverTests::checkSame, "Incorrect results returned from sameContinent()");
	tests["Continents"] = test<RiverTests>(&RiverTests::checkContinents, "getContinents is returning incorrect values");
	tests["Permutation"] = test<RiverTests>(&RiverTests::checkPermutation, "getContinents() breaks when files are loaded in different order");
	tests["Bogus River"] = test<RiverTests>(&RiverTests::checkBogusRiver, "getContinents() should return \"\" for invalid data");
	tests["Bogus Pair"] = test<RiverTests>(&RiverTests::checkBogusPair, "sameContinent() should return false when either river is invalid");
	tests["Hardcoded"] = test<RiverTests>(&RiverTests::checkHardcoded, "HARDCODED DATA IN FILES");

	for (auto i : files)
		if (std::filesystem::exists(i + ".bak"))
			std::filesystem::rename(i + ".bak", i);
}

bool RiverTests::checkHardcoded() {
	for (auto i : std::filesystem::recursive_directory_iterator("."))
		if (std::filesystem::path(i).extension() == ".cpp" || std::filesystem::path(i).extension() == ".h")
		{
			if (std::filesystem::path(i).stem().string().find("Feedback (DO NOT EDIT)") == std::string::npos)
			{
				std::ifstream ifs(std::filesystem::path(i).string());
				std::string line;
				while (std::getline(ifs, line)) {
					if (line.find("Yangtze") != std::string::npos || line.find("Limpopo") != std::string::npos || line.find("Yenisei") != std::string::npos) {
						std::cout << "Line found containing hardcoded river data: " << std::endl << line << std::endl << " in file " << std::filesystem::path(i).string() << std::endl << std::endl;
						return false;
					}
				}
			}
		}
	return true;
}

bool RiverTests::checkRand() {
	time_t t1 = time(0);
	histogram.clear();
	for (int i = 0; i < samples; i++) {
		std::string river = r.getRandomRiver();
		if (histogram.find(river) == histogram.end())
			histogram[river] = 0;
		++histogram[river];
	}
	time_t elapsed = time(0) - t1;
	for (auto i : histogram)
		if (i.second >= samples / (elapsed + 1))
			return false;
	return true;
}

bool RiverTests::checkBogusRiver()
{
	return r.getContinent("Invalid") == "";
}

bool RiverTests::checkBogusPair()
{
	return !r.sameContinent("Invalid", "AlsoInvalid");
}

bool RiverTests::checkAllRivers() {

	for (auto i : riverData)
		for (auto j : i.second) {
			bool found = false;
			for (auto k : histogram)
				if (k.first == j)
					found = true;
			if (!found) {
				return false;
			}
		}
	return true;
}

bool RiverTests::checkSame() {
	for (auto i : riverData)
		for (auto j : i.second)
			for (auto k : riverData) {
				if (k != i) {
					for (auto l : k.second)
						if (r.sameContinent(j, l))
							return false;
				}
				else {
					for (auto l : k.second)
						if (!r.sameContinent(j, l))
							return false;
				}
			}
	return true;
}

bool RiverTests::checkFiles()
{
	tests["Continents"](*this);
	tests["Same"](*this);
	Renamer ren;
	try {
		if(!tests["Random"].fnc(*this))
			return false;
		if(!tests["All Rivers"].fnc(*this))
			return false;
		if(!tests["Same"].fnc(*this) && tests["Same"].passed)
			return false;
		if(!tests["Continents"].fnc(*this) && tests["Continents"].passed)
			return false;
	}
	catch (std::exception e) {
		return false;
	}
	return true;
}

bool RiverTests::checkContinents()
{
	for (auto i : riverData)
		for (auto j : i.second)
			if (r.getContinent(j) != i.first)
				return false;
	return true;
}

bool RiverTests::checkPermutation()
{
	std::vector<std::string> files2 = files;
	std::swap(*files2.begin(), *files2.rbegin());
	Rivers r2(files2);
	for (auto i : riverData)
		for (auto j : i.second)
			if (r2.getContinent(j) != i.first)
				return false;
	return true;
}

int RiverTests::score() {
	if (!tests["Hardcoded"].passed)
		return 0;
	int result = (tests["Random"].passed & tests["All Rivers"].passed) * 10;
	result += 20 * (tests["Same"].passed & tests["Bogus Pair"].passed);
	if (tests["Bogus River"].passed) {
		result += tests["Continents"].passed * 10;
		result += tests["Permutation"].passed * 10;
	}
	if (!tests["Files"].passed)
		result -= 30;
	return std::max(0, result);
}



class GameTests : public TestSuite<GameTests>{
	Rivers r; 
	Game g;
	bool checkScore();
	bool checkTermination();
	bool checkTotal();
	bool checkContinuationS();
	bool checkContinuationD();
	bool checkCorrect();
	bool checkContinents();
	int rounds = 250;
	static const std::vector<std::string> rivers;
public:
	GameTests();
	virtual void results(std::ostream& o) { o << "\nGame UI - issue list:  "; TestSuite<GameTests>::results(o); }
	int score();
};

GameTests::GameTests() : r(files), g(r) {
	tests["Score"] = test<GameTests>(&GameTests::checkScore, "Scores appear wrong or inconsistent with sameContinent()");
	tests["Total"] = test<GameTests>(&GameTests::checkTotal, "Total not counted correctly");
	tests["Termination"] = test<GameTests>(&GameTests::checkTermination, "playRound() should return false after 'q' has been entered");
	tests["Continuation S"] = test<GameTests>(&GameTests::checkContinuationS, "playRound() should return true after 's' has been entered");
	tests["Continuation D"] = test<GameTests>(&GameTests::checkContinuationD, "playRound() should return true after 'd' has been entered");
	tests["Correct"] = test<GameTests>(&GameTests::checkCorrect, "playRound() should output 'correct' on correct answer and 'incorrect' otherwise");
	tests["Continents"] = test<GameTests>(&GameTests::checkContinents, "playRound() should output the names of the continents after the answer is entered");
}

void parseRivers(std::string txt, std::vector<std::string>& rivers, Rivers& r) {
	std::istringstream is(txt);
	std::vector<std::string> lines;
	std::string line;
	is >> std::ws;
	while (std::getline(is, line)) {
		lines.push_back(line);
		is >> std::ws;
	}
	for (auto i : lines)
		if (r.getContinent(i).size() != 0)
			rivers.push_back(i);
}


std::string makeInput(char input) {
	std::ostringstream inputMaker;
	inputMaker << input << std::endl;
	return inputMaker.str();
}

bool GameTests::checkCorrect() {
	bool right = false;
	bool wrong = false;
	for (int c = 0; !right || !wrong; c++) {
		std::ostringstream os;
		std::istringstream is(makeInput('d'));
		g.playRound(os, is);
		std::string output = os.str();
		std::vector<std::string> rivers;
		parseRivers(output, rivers, r);
		if (rivers.size() < 2)
			return false;
		std::transform(output.begin(), output.end(), output.begin(), [](unsigned char c) {return std::tolower(c); });

		if (!r.sameContinent(rivers[0], rivers[1])) {
			if (output.find("correct") != std::string::npos && output.find("incorrect") == std::string::npos)
				right = true;
			else return false;
		}
		else {
			//handle incorrect
			if (output.find("incorrect") != std::string::npos)
				wrong = true;
			else return false;
		}
		if (c > 100) return false;
	}
	return true;
}

bool GameTests::checkContinents() {
	std::ostringstream os;
	std::istringstream is(makeInput('s'));
	g.playRound(os, is);
	std::string output = os.str();
	std::vector<std::string> rivers;
	parseRivers(output, rivers, r);
	if (rivers.size() < 2)
		return false;
	std::string c1 = r.getContinent(rivers[0]);
	std::string c2 = r.getContinent(rivers[1]);
	if (output.find(c1) == std::string::npos || output.find(c2) == std::string::npos)
		return false;
	for (auto i : files)
		if (i.find(c1) == std::string::npos && output.find(c2) == std::string::npos)
			if (output.find(std::filesystem::path(i).stem().string()) != std::string::npos)
				return false;
	return true;
}

bool GameTests::checkScore() {
	if (r.sameContinent("", ""))
		return false;
	if (!r.sameContinent("Yangtze", "Yangtze"))
		return false;
	g.reset();
	int score = 0;
	for (int i = 0; i < rounds; ++i)
	{
		std::ostringstream os;
		std::istringstream is(makeInput('d'));
		g.playRound(os, is);
		std::vector<std::string> rivers;
		parseRivers(os.str(), rivers, r);
		if (rivers.size() < 2)
			return false;
		if (rivers.size() > 1 && !r.sameContinent(rivers[0], rivers[1]))
			score++;
	}
	std::ostringstream os;
	std::istringstream is(makeInput('q'));
	g.playRound(os, is);
	return  g.getScore() == score;
}

bool GameTests::checkTotal() {
	g.reset();
	std::ostringstream os;

	for (int i = 0; i < rounds; ++i)
	{
		std::istringstream is(makeInput('d'));
		g.playRound(os, is);
	}
	std::istringstream is(makeInput('q'));
	g.playRound(os, is);
	return g.getTotal() == rounds;
}

bool GameTests::checkTermination() {
	std::ostringstream os;
	std::istringstream is(makeInput('q'));
	return !g.playRound(os, is);
}

bool GameTests::checkContinuationS() {
	std::ostringstream os;
	std::istringstream is(makeInput('s'));
	return g.playRound(os, is);
}

bool GameTests::checkContinuationD() {
	std::ostringstream os;
	std::istringstream is(makeInput('d'));
	return g.playRound(os, is);
}


int GameTests::score() {
	return tests["Score"].passed * 5 + (tests["Continents"].passed & tests["Correct"].passed) * 5 + tests["Total"].passed * 5 + (tests["Termination"].passed & tests["Continuation S"].passed & tests["Continuation D"].passed) * 10;
}

class AdvancedTests : public TestSuite<AdvancedTests> {
	Rivers r;
	Game g;
	bool checkSixes();
	bool checkPause();
	bool checkSorted();
	bool checkMode1();
	bool checkMode2();
	bool checkMode3();
public:
	AdvancedTests();
	virtual void results(std::ostream& o);
	int score();
};

AdvancedTests::AdvancedTests() :r(files), g(r) {
	tests["Sixes"] = test<AdvancedTests>(&AdvancedTests::checkSixes, "getRandomRiver() should not repeat a value until 6 calls have elapsed");
	tests["Sorted"] = test<AdvancedTests>(&AdvancedTests::checkSorted, "getFastest() should output 5 fastest correct answers in nonincreasing order of speed");
	tests["Mode 1"] = test<AdvancedTests>(&AdvancedTests::checkMode1, "Random mode 1 should create an equal probability of each continent being selected");
	tests["Mode 2"] = test<AdvancedTests>(&AdvancedTests::checkMode2, "Random mode 2 should create an equal probability of each river being selected");
	tests["Mode 3"] = test<AdvancedTests>(&AdvancedTests::checkMode3, "Random mode 3 should create a 50% chance of two consecutive rivers being from the same continent");

#ifdef RUN_PAUSE_TEST
	tests["Pause"] = test<AdvancedTests>(&AdvancedTests::checkPause, "Answers should be scored as incorrect if the user takes over 10 seconds to enter");
#endif
}

void AdvancedTests::results(std::ostream& o) {
	TestSuite<AdvancedTests>::results(o);
#ifndef RUN_PAUSE_TEST
	o << "\033[0;33m\n\tPause test not executed: uncomment '#define RUN_PAUSE_TEST' in Game.h to enable it\033[0m";
#endif
}

bool AdvancedTests::checkSixes() {
	std::ofstream ofs("dummy.txt", std::ios::out);
	std::ofstream ofs1("dummy2.txt", std::ios::out);
	for (int i = 0; i < 4; ++i)
		ofs << i << std::endl;
	for (int i = 4; i < 7; ++i)
		ofs1 << i << std::endl;
	ofs.close();
	ofs1.close();
	std::vector<std::string> fs = { "dummy.txt", "dummy2.txt"};
	Rivers sixRivers(fs);
	std::vector<std::string> gens(100);
	for (int i = 0; i < gens.size(); i++) {
		gens[i] = sixRivers.getRandomRiver();
		bool found = false;
		for (int j = 0; j < 7; ++j)
		{
			std::ostringstream os;
			os << j;
			if (os.str() == gens[i])
				found = true;
		}
		if(!found)
			return false;
	}
	for (int i = 7; i < gens.size(); i++) {
		if (gens[i] != gens[i - 7])
			return false;
		if (gens[i] == gens[i - 1])
			return false;
	}
	std::filesystem::remove("dummy.txt");
	std::filesystem::remove("dummy2.txt");
	return true;
}

class delayBuf : public std::streambuf {
	std::streambuf* buf;
	char ch;
	int delay_ms;
protected:
	virtual std::streambuf::int_type underflow() {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
		std::streambuf::int_type res = buf->sbumpc();
		if (res != traits_type::eof() && !traits_type::eq(traits_type::to_char_type(res),'\n')) {
			ch = traits_type::to_char_type(res);
			setg(&ch, &ch, &ch + 1);
		}
		return res;
	}
public:
	delayBuf(std::streambuf* bf, int delay_millis) : buf(bf), delay_ms(delay_millis),ch(0) {}
	virtual ~delayBuf() {}
};
class delayStream : public std::istream {
public:
	delayStream(std::istream& is, int delay_millis) : std::istream(new delayBuf(is.rdbuf(), delay_millis)) {}
};

bool AdvancedTests::checkPause() {
	std::cout << "\nPause test(s) executing. Please wait..." << std::flush;
	g.reset();
	bool gotOne = false;
	while (!gotOne) {
		std::ostringstream os;
		std::istringstream is(makeInput('d'));
		delayStream ds(is, 10001);
		g.playRound(os, ds);
		std::string output = os.str();
		std::vector<std::string> rivers;
		parseRivers(output, rivers, r);
		if (rivers.size() < 2)
			return false;
		if (!r.sameContinent(rivers[0], rivers[1])) {
			gotOne = true;
			if (output.find("ime limit exceeded") == std::string::npos)
				return false;
		}
	}
	while (gotOne) {
		std::ostringstream os;
		std::istringstream is(makeInput('d'));
		g.playRound(os, is);
		std::string output = os.str();
		std::vector<std::string> rivers;
		parseRivers(output, rivers, r);
		if (rivers.size() < 2)
			return false;
		if (!r.sameContinent(rivers[0], rivers[1]))
			gotOne = false;
	}
	return g.getScore() == 1;
}

bool AdvancedTests::checkSorted() {
	std::vector<std::string> rivs;
	for (int i = 0; i < 20; ++i)
	{
		std::ostringstream os;
		char c = rand() % 4 ? 'd' : 's';
		std::istringstream is(makeInput(c));
		delayStream ds(is, rand() % 30);
		g.playRound(os, ds);
		std::string output = os.str();
		std::vector<std::string> rivers;
		parseRivers(output, rivers, r);
		if (rivers.size() < 2)
			return false;
		if (c == 'd' &&  !r.sameContinent(rivers[0], rivers[1]) || c == 's' && r.sameContinent(rivers[0],rivers[1])) {
			rivs.push_back(rivers[0]);
			rivs.push_back(rivers[1]);
		}
	}
	std::vector<std::string> sorts = g.getFastest();
	if (sorts.size() > 5)
		return false;
	double last(-1);
	for (auto i : sorts) {
		std::istringstream is(i);
		std::string line, line2;
		double cur;
		std::getline(is, line, ',');
		if (r.getContinent(line) == "")
			return false;
		std::getline(is, line2, ',');
		if (r.getContinent(line2) == "")
			return false;
		bool found = false;
		for (int i = 0; i < rivs.size(); i += 2) {
			if (rivs[i] == line && rivs[i + 1] == line2 || rivs[i] == line2 && rivs[i + 1] == line)
				found = true;
		}
		if (!found)
			return false;
		is >> cur;
		if (cur < last)
			return false;
		last = cur;
	}
	return true;
}

bool AdvancedTests::checkMode1() {
	const int rounds = 10000;
	const int variance_bound = 40000;
	r.setMode(1);
	std::unordered_map<std::string, int> histogram;
	for (int i = 0; i < rounds; ++i) {
		std::string continent = r.getContinent(r.getRandomRiver());
		if (continent == "")
			return false;
		if (!histogram.contains(continent))
			histogram[continent] = 1;
		else histogram[continent]++;
	}
	if (histogram.size() != files.size())
		return false;
	double mean=0;
	for (auto i : histogram)
		mean += i.second;
	mean /= histogram.size();
	double variance = 0;
	for (auto i : histogram)
		variance += (i.second - mean) * (i.second - mean);
	return variance/histogram.size() < variance_bound; 
}

bool AdvancedTests::checkMode2() {
	r.setMode(2);
	const int rounds = 20000;
	const double variance_bound = 500;
	std::unordered_map<std::string, int> histogram;
	for (int i = 0; i < rounds; ++i) {
		std::string river = r.getRandomRiver();
		if (!histogram.contains(river))
			histogram[river] = 1;
		else histogram[river]++;
	}
	double mean = 0;
	for (auto i : histogram)
		mean += i.second;
	mean /= histogram.size();
	double variance = 0;
	for (auto i : histogram)
		variance += (i.second - mean) * (i.second - mean);
	return variance / histogram.size() < variance_bound; 
}

bool AdvancedTests::checkMode3() {
	const int rounds = 1000;
	const int square_difference_bound = 75000;
	r.setMode(3);
	std::string r1 = r.getRandomRiver();
	int histogram[2] = { 0,0 };
	for (int i = 0; i < rounds; ++i) {
		std::string r2 = r.getRandomRiver();
		if (r.sameContinent(r1, r2))
			histogram[0]++;
		else
			histogram[1]++;
		r1 = r2;
	}
	return (histogram[1] - histogram[0]) * (histogram[1] - histogram[0]) < square_difference_bound;
}

int AdvancedTests::score() {
	return tests["Pause"].passed * 10 + tests["Sorted"].passed * 10 + tests["Sixes"].passed * 10 + (tests["Mode 1"].passed & tests["Mode 2"].passed & tests["Mode 3"].passed) * 10;
}


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

int main()
{
#ifdef _WIN32
	setConsoleVTMode();
#endif

#ifdef ASSIGNMENT_2
	std::cout << "Legacy feedback from prototype: " << std::endl;
#endif 
	std::unique_ptr<RiverTests> Rtests = std::make_unique<RiverTests>();
	(*Rtests)();
	Rtests->results(std::cout);
#ifndef ASSIGNMENT_2
	std::cout << "\nScore " << Rtests->score() << " / 50" << std::endl;
#endif
	if (!Rtests->noHardcoded())
		return 0;
	std::unique_ptr<GameTests> Gtests = std::make_unique<GameTests>();
	(*Gtests)();
	Gtests->results(std::cout);
#ifndef ASSIGNMENT_2
	std::cout << "\nScore " << Gtests->score() << " / 25" << std::endl;
#endif
#ifdef ASSIGNMENT_2
	
	std::cout << "\n\nProject feedback:";
	std::unique_ptr<AdvancedTests> Atests = std::make_unique<AdvancedTests>();
	(*Atests)();
	Atests->results(std::cout);
	std::cout << "\nScore " << Atests->score() << " / 40" << std::endl;
#endif
	std::cin.get();
	return 0;
}