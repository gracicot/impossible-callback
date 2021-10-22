#include <impossible-callback/impossible-callback.hpp>

#include <cstdio>

struct C {
	char c = 'c';
};

struct B {
	char b = 'b';

	virtual void function() {
		std::printf("from B %p: %c\n", this, b);
	}
};

struct A : C, virtual B {
	char a = 'a';

	void function() override {
		std::printf("from A %p: %c\n", this, a);
	}
};

int main() {
	A a;
	// prints "Expects 0x7ffdbf172b2f"
	std::printf("Expects %p\n", static_cast<B*>(&a));
	auto [object, function] = ic::impossible_callback(&a, &B::function);

	// object is an impossible_object pointer
	static_assert(std::is_same_v<decltype(object), ic::impossible_object*>);

	// type of function is a function pointer that takes an impossible_object
	static_assert(std::is_same_v<decltype(function), void(*)(ic::impossible_object*)>);

	// Call member function through normal function pointer!
	function(object);
	return 0;
}
