#pragma once

#include <memory>
#include <impossible-callback/impossible-callback.hpp>
#include <random>
#include <functional>

#include "zoo/AnyCallable.h"
#include "zoo/function.h"

constexpr auto amount = 1021;
constexpr auto reiterations = 11;

struct Base {
	virtual auto function(std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) const -> std::int64_t = 0;
	virtual ~Base() = default;
};

struct D1 : Base {
	auto function(std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) const -> std::int64_t override {
		return arg1 ^ arg2 ^ (p1 - p2);
	}
};

struct D2 : Base {
	auto function(std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) const -> std::int64_t override {
		return p1 - p2 + arg1 - arg2;
	}
};

struct Simple {
	auto function1(std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) const -> std::int64_t {
		return arg1 ^ arg2 ^ (p1 - p2);
	}

	auto function2(std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) const -> std::int64_t {
		return p1 - p2 + arg1 - arg2;
	}
};

auto free_function1(std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) -> std::int64_t {
	return arg1 ^ arg2 ^ (p1 - p2);
}

auto free_function2(std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) -> std::int64_t {
	return p1 - p2 + arg1 - arg2;
}

using function_pointer = auto(*)(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*) -> std::int64_t;
using virtual_member_function_pointer = auto(Base::*)(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*) const -> std::int64_t;
using member_function_pointer = auto(Simple::*)(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*) const -> std::int64_t;
using impossible_return = std::pair<impossible_callback::impossible_object const*, impossible_callback::impossible_function_const<std::int64_t, std::int64_t, std::int64_t, std::int32_t*, std::int32_t*>>;

inline auto get_random_class(int one_over_x) -> std::unique_ptr<Base> {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, one_over_x - 1);

	switch (dis(gen)) {
		case 0:
			return std::make_unique<D1>();
		default:
			return std::make_unique<D2>();
	}
}

inline auto get_random_function(int one_over_x) -> function_pointer {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, one_over_x - 1);

	switch (dis(gen)) {
		case 0:
			return &free_function1;
		default:
			return &free_function2;
	}
}

inline auto get_random_member_function(int one_over_x) -> member_function_pointer {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, one_over_x - 1);

	switch (dis(gen)) {
		case 0:
			return &Simple::function1;
		default:
			return &Simple::function2;
	}
}

inline auto get_random_std_function(int one_over_x) {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, one_over_x - 1);

	switch (dis(gen)) {
		case 0:
			return std::function{[s = Simple{}](std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) { return s.function1(arg1, arg2, p1, p2); }};
		default:
			return std::function{[s = Simple{}](std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) { return s.function2(arg1, arg2, p1, p2); }};
	}
}

inline auto get_random_std_function_virtual(int one_over_x) {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, one_over_x - 1);

	switch (dis(gen)) {
		case 0:
			return std::function{[d = D1{}](std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) { return d.function(arg1, arg2, p1, p2); }};
		default:
			return std::function{[d = D2{}](std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) { return d.function(arg1, arg2, p1, p2); }};
	}
}

inline auto get_random_std_function_fptr(int one_over_x) {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, one_over_x - 1);

	switch (dis(gen)) {
		case 0:
			return std::function{free_function1};
		default:
			return std::function{free_function2};
	}
}


inline auto get_random_zoo_function(int one_over_x) {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, one_over_x - 1);

	switch (dis(gen)) {
		case 0:
			return zoo::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>{[s = Simple{}](std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) { return s.function1(arg1, arg2, p1, p2); }};
		default:
			return zoo::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>{[s = Simple{}](std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) { return s.function2(arg1, arg2, p1, p2); }};
	}
}

inline auto get_random_zoo_function_virtual(int one_over_x) {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, one_over_x - 1);

	switch (dis(gen)) {
		case 0:
			return zoo::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>{[d = D1{}](std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) { return d.function(arg1, arg2, p1, p2); }};
		default:
			return zoo::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>{[d = D2{}](std::int64_t arg1, std::int64_t arg2, std::int32_t* p1, std::int32_t* p2) { return d.function(arg1, arg2, p1, p2); }};
	}
}

inline auto get_random_zoo_function_fptr(int one_over_x) {
	std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, one_over_x - 1);

	switch (dis(gen)) {
		case 0:
			return zoo::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>{free_function1};
		default:
			return zoo::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>{free_function2};
	}
}


inline auto get_callback(Base const* base) -> impossible_return {
	auto const [object, function] = impossible_callback::impossible_callback(base, &Base::function);
	return {object, function};
}

inline auto get_callback(function_pointer fptr) -> impossible_return {
	auto const [object, function] = impossible_callback::impossible_callback(fptr, impossible_callback::as_const);
	return {object, function};
}

inline auto get_callback(Simple const* o, member_function_pointer mf) -> impossible_return {
	auto const [object, function] = impossible_callback::impossible_callback(o, mf);
	return {object, function};
}
