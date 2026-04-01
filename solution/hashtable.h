#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <initializer_list>
#include <utility>
#include <optional>


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

template<typename valType, int s>
class hashtable {
private:
	std::vector< hashobject<valType>* > table;
	const int size = s;
public:
	inline hashtable() { table.resize(s); };
	inline hashtable(std::initializer_list< std::pair<std::string, valType> > newTable) {
		table.resize(s);
		if (newTable.size() == s) {
			throw "input value is not the same size as specified size";
		}
		for (auto it = newTable.begin(); it != newTable.end(); ++it) {
			hashobject<valType>* newhash = new hashobject<valType>;

			newhash->key = it->first;
			newhash->value = it->second;
			int strhash = hash(it->first, s);
			if (table[strhash] == nullptr) {
				table[strhash] = newhash;
			}
			else {
				hashobject<valType>* parent = nullptr;
				hashobject<valType>* child = table[strhash];
				while (child != nullptr) {
					parent = child;
					child = parent->child;
				}
				newhash->parent = parent;
				parent->child = newhash;
			}

		}
	};

	inline int add(std::pair<std::string, valType> newPair) {
		hashobject<valType>* newhash = new hashobject<valType>;

		newhash->key = newPair.first;
		newhash->value = newPair.second;
		int strhash = hash(newPair.first, s);
		if (table[strhash] == nullptr) {
			table[strhash] = newhash;
		}
		else {
			hashobject<valType>* parent = nullptr;
			hashobject<valType>* child = table[strhash];
			while (child != nullptr) {
				parent = child;
				child = parent->child;
			}
			newhash->parent = parent;
			parent->child = newhash;
		}
	};

	inline hashobject<valType>* find(std::string key) {
		int strhash = hash(key, s);
		hashobject<valType>* it = table[strhash];
		while (it != nullptr) {
			if (it->key == key) {
				return it;
			}
			it = it->child;
		}
		return nullptr;
	}
};
