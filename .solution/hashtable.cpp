#include "hashtable.h"

/*
template<>
inline int hash<const char*>(const char* key, int s) {
	size_t size = strlen(key);
	int total = 0;
	for (size_t i = 0; i != size; ++i) {
		total += static_cast<int>(key[i]);
	}
	return total % s;
}
*/

template<>
inline int hash<std::string>(std::string key, int s) {
	int total = 0;
	for (auto it = key.begin(); it != key.end(); ++it) {
		total += static_cast<int>(*it);
	}
	return total % s;
}