#pragma once

#include <memory>
#include <impossible-callback/impossible-callback.hpp>

struct Base {
	virtual auto function() const -> int = 0;
	virtual ~Base() = default;
};

struct Simple {
	auto function1() const -> int;
	auto function2() const -> int;
	auto function3() const -> int;
	auto function4() const -> int;
};

using function_pointer = auto(*)() -> int;
using member_function_pointer = auto(Simple::*)() const -> int;
using impossible_return = std::pair<impossible_callback::impossible_object const*, impossible_callback::impossible_function_const<int>>;

auto get_random_class() -> std::unique_ptr<Base>;
auto get_random_function() -> function_pointer;
auto get_random_member_function() -> member_function_pointer;
auto get_callback(Base const* base) -> impossible_return;
