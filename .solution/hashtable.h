#pragma once

#include <string>
#include <iostream>
#include <vector>

/*
probably just have this implemented for characters only
*/

int hash(std::string value, int s);

template<typename valType>
struct hashobject {
	std::string key = "";
	valType value;
	hashobject parent;
	hashobject child;
};

template<typename valType>
class hashtable {
private:
	std::vector<hashobject<valType>> table = { 0 };
	const int size;
public:
	inline hashtable(int s) {
		table.resize(s);
	};
	inline add(std::string key, valType value) {
		int keyHash = hash(key, size);
		if (table[keyHash].key == "") {
			table[keyHash] = newobject;
		}
		hashobject<valType>* it = table[keyHash].child;
		for (int i = 0; i < 5; ++i) {
			if (it == nullptr) {
				table[keyHash]
			}
		}
	};

};
