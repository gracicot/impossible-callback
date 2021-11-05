#include <impossible-callback/impossible-callback.hpp>

#include <functional>
#include <cstdio>

struct A {
	char a = 'a';

	void function() {
		std::printf("from A %p: %c\n", this, a);
	}
};

int main() {
	A a;

	// Print the address of a, for good mesure.
	std::printf("Address of a: %p\n", &a);

	// Get a `impossible_object*` and a `impossible_function<void>`
	auto [object, function] = impossible_callback::impossible_callback(&a, &A::function);

	// Call member function through normal function pointer, just as if a.function(). (not on msvc, more on that later)
	std::invoke(function, object);
}
