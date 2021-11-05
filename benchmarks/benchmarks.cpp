#include <benchmark/benchmark.h>

#include <functional>

#include "bench-helper.hpp"

static void BM_virtual_call(benchmark::State& state) {
	auto pointers = std::vector<std::unique_ptr<Base>>{};
	pointers.resize(2000);
	std::generate(pointers.begin(), pointers.end(), []{ return get_random_class(); });

	for (auto _ : state)
		for (auto const& ptr : pointers)
			benchmark::DoNotOptimize(ptr->function());
}

// Register the function as a benchmark
BENCHMARK(BM_virtual_call);

static void BM_function_pointer(benchmark::State& state) {
	auto pointers = std::vector<function_pointer>{};
	pointers.resize(2000);
	std::generate(pointers.begin(), pointers.end(), []{ return get_random_function(); });

	for (auto _ : state)
		for (auto const fptr : pointers)
			benchmark::DoNotOptimize(fptr());
}

BENCHMARK(BM_function_pointer);

static void BM_member_function_pointer(benchmark::State& state) {
	auto pointers = std::vector<std::pair<Simple*, member_function_pointer>>{};
	pointers.resize(2000);
	std::generate(pointers.begin(), pointers.end(), []{ return std::pair{new Simple, get_random_member_function()}; });

	for (auto _ : state)
		for (auto const& [object, mptr] : pointers)
			benchmark::DoNotOptimize((object->*mptr)());

	for (auto& [object, mptr] : pointers) {
		delete object;
	}
}

BENCHMARK(BM_member_function_pointer);

// Define another benchmark
static void BM_impossible_call(benchmark::State& state) {
	auto pointers = std::vector<std::unique_ptr<Base>>{};
	pointers.resize(2000);
	std::generate(pointers.begin(), pointers.end(), []{ return get_random_class(); });

	auto callbacks = std::vector<impossible_return>{};
	callbacks.resize(2000);
	std::transform(
		pointers.begin(), pointers.end(),
		callbacks.begin(),
		[](auto const& ptr) { return get_callback(ptr.get()); }
	);

	for (auto _ : state)
		for (auto const& [object, function] : callbacks)
			benchmark::DoNotOptimize(function(object));
}

// Register the function as a benchmark
BENCHMARK(BM_impossible_call);
