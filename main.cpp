#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <stdio.h>
#include <string.h>
#include <stdexcept>      // std::invalid_argument
#include "GeeMail.hh"
#include <random>
#include <utility>      // std::pair, std::make_pair

using namespace std;

bool argumentCheck(int width, int height, int number_of_mines) {
	// neither arguments can be zero
	if (width == 0 || height == 0 || number_of_mines == 0) {
		cout << "argument error: please give correct argument " << endl;
		return false;
	}

	// number_of_mines must be less than total number of cell
	// need overflow check here
	if (number_of_mines >= width * height) {
		cout << "argument error: please give correct argument" << endl;
		return false;
	}
	return true;

}

// ERR62-CPP. Detect errors when converting a string to a number
bool StringToInt(const string& str, int& result) {
	try {
		size_t lastChar;
		result = stoi(str, &lastChar, 10);
		return lastChar == str.size();
	} catch (invalid_argument&) {
		return false;
	} catch (out_of_range&){
		return false;
	}
}

int main()
{	
	
	GeeMail *geeMail = new GeeMail();
	geeMail->start();
	return 0;
}