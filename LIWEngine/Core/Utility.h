#pragma once
#include <cstdlib>
#include <map>
#include <algorithm>
#include <memory>

#include "LIWMacros.h"
#include "Exceptions.h"

using namespace std;

class Utility {
public:
	template<class K, class V>
	static bool IsKeyInMap(const map<K, V>& m, const K& key) {
		return m.find(key) != m.end();
	}
};