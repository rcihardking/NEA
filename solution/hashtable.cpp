#include "hashtable.h"

int hash(std::string val, int s) {
	int total = 0;
	for (auto it = val.begin(); it != val.end(); ++it) {	//add all the decimal values of the characters together to create hash
		total += static_cast<int>(*it);
	}
	return total % s;
}