#pragma once

template<typename T>
struct hashObject {
	T value;
	hashObject* child;
	hashObject* parent;
};

template<typename T, int size>
class hashtable {
public:
	inline hashtable() {
		table = new hashObject[size];
	};
	inline ~hashtable() {
		delete table[size];
		table = nullptr;
	};
	inline int add(T value) {
		int memaddress = reinterpret_cast<int>(&value);
		int loc = memaddress % size;
	};
	inline void remove(T value);
	inline bool search(T value);
private:
	hashObject<T>* table;
};