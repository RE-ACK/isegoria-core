#pragma once

#include <atomic>
#include <cstdint>

class IdGenerator {
public:
	static uint64_t generate() {
		return ++counter_;
	}

private:
	inline static std::atomic<uint64_t> _counter{ 0 };
};