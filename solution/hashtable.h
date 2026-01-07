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
	hashobject* child = nullptr;
	hashobject* parent = nullptr;
};

template<typename valType>
class hashtable {
private:
	std::vector<hashobject<valType>*> table = { 0 };
	int size;
public:
	inline hashtable() {};
	inline hashtable(int s) : size{ s } {
		table.resize(size);
	};
	inline ~hashtable() { // deletes all hashtable heap variables
		for (int i = 0; i < table.size(); ++i) {
			hashobject<valType>* it = table[i];
			if (it != nullptr) {
				for (int i = 0; i < 3; ++i) {
					if (it->child == nullptr) {
						break;
					}
					it = it->child;
				}
				for (int i = 0; i < 3; ++i) {
					if (it->parent == nullptr) {
						delete it;
						break;
					}
					it = it->parent;
					delete it->child;
				}
			}
		}
	}
	inline int resize(int s) { // only use when empty
		size = s;
		table.resize(size);
	}
	inline int add(std::string key, valType value) {
		hashobject<valType>* newobject = new hashobject<valType>;
		newobject->key = key;
		newobject->value = value;
		int keyHash = hash(key, size);

		if (table[keyHash] == nullptr) {
			table[keyHash] = newobject;
			return 0;
		}
		hashobject<valType>* it = table[keyHash];
		for (int i = 0; i < 3; ++i) { // maximum of 3 collisions
			if (it->child == nullptr) {
				it->child = newobject;
				newobject->parent = it;
				return 1;
			}
			it = newobject->child;
		}
		return 2; // failed to add to hashlist due to too many collisions
	};
	inline hashobject<valType>* get(std::string key) {
		int keyHash = hash(key, size);
		if (table[keyHash] == nullptr) {
			return nullptr;
		}
		hashobject<valType>* it = table[keyHash];
		for (int i = 0; i < 3; ++i) {
			if (it->key == key) {
				return it;
			}
			it = it->child;
		}
		return nullptr; // past collision limit
	}
};
