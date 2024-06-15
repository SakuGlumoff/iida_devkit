#include "error.hpp"

extern "C" void Panic() {
	// TODO: Handle this better.
	while (true) {}
}
