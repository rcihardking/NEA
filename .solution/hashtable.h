#pragma once

#include <string>
#include <iostream>
#include <vector>

/*
probably just have this implemented for characters only
*/

// currently no hashing (i can think of) algorithm that works for every type
template<typename T>
int hash(T key, int s) { exit(0); return 0; };


template<typename keyType, typename valType>
struct hashObject {
	keyType key;
	valType value;
	hashObject* child;
	hashObject* parent;
};

template<typename keyType, typename valType>
class hashtable {
public:
	const int size;

	inline hashtable(int s) : size{ s } {
		table.resize(s);
		occupied.resize(s);
	};
	inline int add(keyType key, valType val) {  
		hashObject<keyType, valType> newObject;
		newObject.key = key;
		newObject.value = val;

		int keyHash = hash<keyType>(key, size);
		if (occupied[keyHash] == 0) {
			table[keyHash] = newObject;
			occupied[keyHash] = 1;
			return 0;
		}
		hashObject<keyType, valType>* it = &table[keyHash];
		while (true) {
			if (it->child == nullptr) {
				break;
			}
			it = it->child;
		}
		newObject.parent = it;
		it->child = &newObject;
		return 1;
	};
	inline void remove(keyType key, valType val) { std::cout << "not implemented\n"; };
	inline valType search(keyType key) {
		int keyHash = hash<keyType>(key, size);
		if (occupied[keyHash] == 0) {
			return 0;
		}
		hashObject<keyType, valType>* it = &table[keyHash];
		while (true) {
			if (it->key == key) {
				return it->value;
			}
			if (it->child == nullptr) {
				return 0;
			}
			it = it->child;
		}
	};
private:
	std::vector<hashObject<keyType, valType>> table;
	std::vector<int> occupied = { 0 };
};