#include "bench-helper.hpp"

#include <random>

struct D1 : Base {
	auto function() const -> int override;
};

struct D2 : Base {
	auto function() const -> int override;
};

struct D3 : Base {
	auto function() const -> int override;
};

struct D4 : Base {
	auto function() const -> int override;
};

auto D1::function() const -> int {
	return 42;
}

auto D2::function() const -> int {
	return 42;
}

auto D3::function() const -> int {
	return 42;
}

auto D4::function() const -> int {
	return 42;
}

auto free_function1() -> int {
	return 42;
}

auto free_function2() -> int {
	return 42;
}

auto free_function3() -> int {
	return 42;
}

auto free_function4() -> int {
	return 42;
}

auto Simple::function1() const -> int {
	return 42;
}

auto Simple::function2() const -> int {
	return 42;
}

auto Simple::function3() const -> int {
	return 42;
}

auto Simple::function4() const -> int {
	return 42;
}

auto get_random_class() -> std::unique_ptr<Base> {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 3);

	switch (dis(gen)) {
		case 0:
			return std::make_unique<D1>();
		case 1:
			return std::make_unique<D2>();
		case 2:
			return std::make_unique<D3>();
		case 3:
			return std::make_unique<D4>();
		default:
			return nullptr;
	}
}

auto get_random_function() -> function_pointer {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 3);

	switch (dis(gen)) {
		case 0:
			return &free_function1;
		case 1:
			return &free_function2;
		case 2:
			return &free_function3;
		case 3:
			return &free_function4;
		default:
			return nullptr;
	}
}

auto get_random_member_function() -> member_function_pointer {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 3);

	switch (dis(gen)) {
		case 0:
			return &Simple::function1;
		case 1:
			return &Simple::function2;
		case 2:
			return &Simple::function3;
		case 3:
			return &Simple::function4;
		default:
			return nullptr;
	}
}

auto get_callback(Base const* base) -> std::pair<impossible_callback::impossible_object const*, impossible_callback::impossible_function_const<int>> {
	auto const [object, function] = impossible_callback::impossible_callback(base, &Base::function);
	return {object, function};
}
