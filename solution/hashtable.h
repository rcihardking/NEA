#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <initializer_list>
#include <utility>
#include <optional>

int hash(std::string value, int s);

template<typename valType>
class hashobject {	//class for linked list in the hashtable
public:
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
	inline hashtable() { table.resize(s); };	//constructor with no agruments for empty initialisation

	inline hashtable(std::initializer_list< std::pair<std::string, valType> > newTable) {	//takes a list with the key and value pairs
		table.resize(s);	//resize array to correct size
		for (auto it = newTable.begin(); it != newTable.end(); ++it) {	//iterate through the passed tables values
			this->add(*it);	//call the add function as to not repeat code
		}
	};

	inline void add(std::pair<std::string, valType> newPair) {
		hashobject<valType>* newhash = new hashobject<valType>;	//create a hashobject on the heap for linked list

		//set the objects key and value to correct data
		newhash->key = newPair.first;
		newhash->value = newPair.second;

		int strhash = hash(newPair.first, s);	//get the keys hash value

		if (table[strhash] == nullptr) {	//if no collision set the new key value pair to that position
			table[strhash] = newhash;
		}
		else {	//if there is a collision iterate down the chain until the bottom, then add hashobject at the bottom
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
		int strhash = hash(key, s);	//turn passed key into hash
		hashobject<valType>* it = table[strhash];
		while (it != nullptr) {
			if (it->key == key) {	//check if the hashobjects key matches the passed key
				return it;
			}
			it = it->child;	//iterate down the linked list
		}
		return nullptr;	//return nothing if reach the bottom of chain without finding matching key
	}
};
