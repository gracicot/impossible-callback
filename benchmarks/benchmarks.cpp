#include <benchmark/benchmark.h>

#include <functional>

#include "bench-helper.hpp"
#include "FastDelegate.h"

static void BM_fptr_std_function(benchmark::State& state) {
	auto functions = std::vector<std::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>>{};
	functions.resize(amount);
	std::generate(functions.begin(), functions.end(), [&]{ return get_random_std_function_fptr(state.range(0)); });

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& function : functions) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(function(l1, l2, p1, p2));
			}
		}
	}
}

BENCHMARK(BM_fptr_std_function)->Arg(2)->Arg(32)->Arg(256);

static void BM_fptr_zoo_function(benchmark::State& state) {
	auto functions = std::vector<zoo::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>>{};
	functions.resize(amount);
	std::generate(functions.begin(), functions.end(), [&]{ return get_random_zoo_function_fptr(state.range(0)); });

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& function : functions) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(function(l1, l2, p1, p2));
			}
		}
	}
}

BENCHMARK(BM_fptr_zoo_function)->Arg(2)->Arg(32)->Arg(256);

static void BM_fptr_don_clugston(benchmark::State& state) {
	using delegate_t = fastdelegate::FastDelegate4<std::int64_t, std::int64_t, std::int32_t*, std::int32_t*, std::int64_t>;
	auto functions = std::vector<delegate_t>{};
	functions.resize(amount);
	std::generate(functions.begin(), functions.end(), [&] {
		return delegate_t{get_random_function(state.range(0))};
	});

	for (auto _ : state) {
		for(auto n = reiterations; n--; ) {
			for (auto const& delegate : functions) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(delegate(l1, l2, p1, p2));
			}
		}
	}
}

BENCHMARK(BM_fptr_don_clugston)->Arg(2)->Arg(32)->Arg(256);

static void BM_fptr_function_pointer(benchmark::State& state) {
	auto pointers = std::vector<function_pointer>{};
	pointers.resize(amount);
	std::generate(pointers.begin(), pointers.end(), [&]{ return get_random_function(state.range(0)); });

	for (auto _ : state) {
		for(auto n = reiterations; n--; ) {
			for (auto const fptr : pointers) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(fptr(l1, l2, p1, p2));
			}
		}
	}
}

BENCHMARK(BM_fptr_function_pointer)->Arg(2)->Arg(32)->Arg(256);

static void BM_fptr_function_pointer_padded(benchmark::State& state) {
	auto pointers = std::vector<std::pair<std::unique_ptr<char>, function_pointer>>{};
	pointers.resize(amount);
	std::generate(pointers.begin(), pointers.end(), [&]{ return std::pair{std::make_unique<char>('a'), get_random_function(state.range(0))}; });

	for (auto _ : state) {
		for(auto n = reiterations; n--; ) {
			for (auto const& [c, fptr] : pointers) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(fptr(l1, l2, p1, p2));
				benchmark::DoNotOptimize(c);
			}
		}
	}
}

BENCHMARK(BM_fptr_function_pointer_padded)->Arg(2)->Arg(32)->Arg(256);

// Define another benchmark
static void BM_fptr_impossible_call(benchmark::State& state) {
	auto callbacks = std::vector<impossible_return>{};
	callbacks.resize(amount);
	std::generate(callbacks.begin(), callbacks.end(), [&] {
		auto const [object, function] = impossible_callback::impossible_callback(get_random_function(state.range(0)), impossible_callback::as_const);
		return impossible_return{object, function};
	});

	for (auto _ : state) {
		for(auto n = reiterations; n--; ) {
			for (auto const& [object, function] : callbacks) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(std::invoke(function, object, l1, l2, p1, p2));
			}
		}
	}
}

// Register the function as a benchmark
BENCHMARK(BM_fptr_impossible_call)->Arg(2)->Arg(32)->Arg(256);


static void BM_memfn_virtual_call(benchmark::State& state) {
	auto pointers = std::vector<std::unique_ptr<Base>>{};
	pointers.resize(amount);
	std::generate(pointers.begin(), pointers.end(), [&]{ return get_random_class(state.range(0)); });

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& ptr : pointers) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(ptr->function(l1, l2, p1, p2));
			}
		}
	}
}

// Register the function as a benchmark
BENCHMARK(BM_memfn_virtual_call)->Arg(2)->Arg(32)->Arg(256);

static void BM_memfn_std_function(benchmark::State& state) {
	auto functions = std::vector<std::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>>{};
	functions.resize(amount);
	std::generate(functions.begin(), functions.end(), [&]{ return get_random_std_function(state.range(0)); });

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& function : functions) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(function(l1, l2, p1, p2));
			}
		}
	}
}

// Register the function as a benchmark
BENCHMARK(BM_memfn_std_function)->Arg(2)->Arg(32)->Arg(256);

static void BM_memfn_zoo_function(benchmark::State& state) {
	auto functions = std::vector<zoo::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>>{};
	functions.resize(amount);
	std::generate(functions.begin(), functions.end(), [&]{ return get_random_zoo_function(state.range(0)); });

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& function : functions) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(function(l1, l2, p1, p2));
			}
		}
	}
}

// Register the function as a benchmark
BENCHMARK(BM_memfn_zoo_function)->Arg(2)->Arg(32)->Arg(256);

static void BM_memfn_don_clugston(benchmark::State& state) {
	using delegate_t = fastdelegate::FastDelegate4<std::int64_t, std::int64_t, std::int32_t*, std::int32_t*, std::int64_t>;
	auto pointers = std::vector<std::unique_ptr<Simple>>{};
	auto functions = std::vector<delegate_t>{};
	functions.resize(amount);
	pointers.resize(amount);
	std::generate(pointers.begin(), pointers.end(), []{ return std::make_unique<Simple>(); });
	std::generate(functions.begin(), functions.end(), [&, nth = 0]() mutable {
		return delegate_t{pointers[nth++].get(), get_random_member_function(state.range(0))};
	});

	for (auto _ : state) {
		for(auto n = reiterations; n--; ) {
			for (auto const& delegate : functions) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(delegate(l1, l2, p1, p2));
			}
		}
	}
}

BENCHMARK(BM_memfn_don_clugston)->Arg(2)->Arg(32)->Arg(256);

static void BM_memfn_member_function_pointer(benchmark::State& state) {
	auto pointers = std::vector<std::pair<std::unique_ptr<Simple>, member_function_pointer>>{};
	pointers.resize(amount);
	std::generate(pointers.begin(), pointers.end(), [&]{
		return std::pair{std::make_unique<Simple>(), get_random_member_function(state.range(0))};
	});

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& [object, mptr] : pointers) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize((object.get()->*mptr)(l1, l2, p1, p2));
			}
		}
	}
}

BENCHMARK(BM_memfn_member_function_pointer)->Arg(2)->Arg(32)->Arg(256);

// Define another benchmark
static void BM_memfn_impossible_call(benchmark::State& state) {
	auto pointers = std::vector<std::unique_ptr<Simple>>{};
	pointers.resize(amount);
	std::generate(pointers.begin(), pointers.end(), []{ return std::make_unique<Simple>(); });

	auto callbacks = std::vector<impossible_return>{};
	callbacks.resize(amount);
	std::transform(
		pointers.begin(), pointers.end(),
		callbacks.begin(),
		[&](auto const& ptr) { return get_callback(ptr.get(), get_random_member_function(state.range(0))); }
	);

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& [object, function] : callbacks) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(std::invoke(function, object, l1, l2, p1, p2));
			}
		}
	}
}

// Register the function as a benchmark
BENCHMARK(BM_memfn_impossible_call)->Arg(2)->Arg(32)->Arg(256);

static void BM_memfn_virtual_std_function(benchmark::State& state) {
	auto functions = std::vector<std::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>>{};
	functions.resize(amount);
	std::generate(functions.begin(), functions.end(), [&]{ return get_random_std_function_virtual(state.range(0)); });

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& function : functions) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(function(l1, l2, p1, p2));
			}
		}
	}
}

// Register the function as a benchmark
BENCHMARK(BM_memfn_virtual_std_function)->Arg(2)->Arg(32)->Arg(256);

static void BM_memfn_virtual_zoo_function(benchmark::State& state) {
	auto functions = std::vector<zoo::function<std::int64_t(std::int64_t, std::int64_t, std::int32_t*, std::int32_t*)>>{};
	functions.resize(amount);
	std::generate(functions.begin(), functions.end(), [&]{ return get_random_zoo_function_virtual(state.range(0)); });

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& function : functions) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(function(l1, l2, p1, p2));
			}
		}
	}
}

// Register the function as a benchmark
BENCHMARK(BM_memfn_virtual_zoo_function)->Arg(2)->Arg(32)->Arg(256);

static void BM_memfn_virtual_don_clugston(benchmark::State& state) {
	using delegate_t = fastdelegate::FastDelegate4<std::int64_t, std::int64_t, std::int32_t*, std::int32_t*, std::int64_t>;
	auto pointers = std::vector<std::unique_ptr<Base>>{};
	auto functions = std::vector<delegate_t>{};
	functions.resize(amount);
	pointers.resize(amount);
	std::generate(pointers.begin(), pointers.end(), [&]{ return get_random_class(state.range(0)); });
	std::generate(functions.begin(), functions.end(), [&, nth = 0]() mutable {
		return delegate_t{pointers[nth++].get(), &Base::function};
	});

	for (auto _ : state) {
		for(auto n = reiterations; n--; ) {
			for (auto const& delegate : functions) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(delegate(l1, l2, p1, p2));
			}
		}
	}
}

BENCHMARK(BM_memfn_virtual_don_clugston)->Arg(2)->Arg(32)->Arg(256);

static void BM_memfn_virtual_member_function_pointer(benchmark::State& state) {
	auto pointers = std::vector<std::pair<std::unique_ptr<Base>, virtual_member_function_pointer>>{};
	pointers.resize(amount);
	std::generate(pointers.begin(), pointers.end(), [&]{
		return std::pair{get_random_class(state.range(0)), &Base::function};
	});

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& [object, mptr] : pointers) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize((object.get()->*mptr)(l1, l2, p1, p2));
			}
		}
	}
}

BENCHMARK(BM_memfn_virtual_member_function_pointer)->Arg(2)->Arg(32)->Arg(256);

// Define another benchmark
static void BM_memfn_virtual_impossible_call(benchmark::State& state) {
	auto pointers = std::vector<std::unique_ptr<Base>>{};
	pointers.resize(amount);
	std::generate(pointers.begin(), pointers.end(), [&]{ return get_random_class(state.range(0)); });

	auto callbacks = std::vector<impossible_return>{};
	callbacks.resize(amount);
	std::transform(
		pointers.begin(), pointers.end(),
		callbacks.begin(),
		[](auto const& ptr) { return get_callback(ptr.get()); }
	);

	for (auto _ : state) {
		for(auto n = reiterations; n--;) {
			for (auto const& [object, function] : callbacks) {
				std::int64_t l1 = 0, l2 = 0;
				std::int32_t i1 = 0, i2 = 0, *p1 = &i1, *p2 = &i2;
				benchmark::DoNotOptimize(std::invoke(function, object, l1, l2, p1, p2));
			}
		}
	}
}

// Register the function as a benchmark
BENCHMARK(BM_memfn_virtual_impossible_call)->Arg(2)->Arg(32)->Arg(256);

