#include "hashtable.h"

int hash(std::string val, int s) {
	int total = 0;
	for (auto it = val.begin(); it != val.end(); ++it) {
		total += static_cast<int>(*it);
	}
	return total % s;
}