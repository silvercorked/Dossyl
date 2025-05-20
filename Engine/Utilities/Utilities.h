#pragma once

#define USE_STL_VECTOR	1
#define USE_STL_DEQUE	1
#if USE_STL_VECTOR
#include <vector>
namespace Dossyl::Util {
	template <typename T>
	using vector = std::vector<T>;

	template <typename T>
	auto eraseUnordered(std::vector<T>& v, size_t index) -> void {
		if (v.size() > 1) {
			std::iter_swap(v.begin() + index, v.end() - 1);
			v.pop_back();
		}
		else {
			v.clear();
		}
	}
}
#endif

#if USE_STL_DEQUE
#include <deque>
namespace Dossyl::Util {
	template <typename T>
	using deque = std::deque<T>;
}
#endif

namespace Dossyl::Util {
	// TODO: implement our own containers
}
