#ifndef IMPOSSIBLE_CALLBACK_HPP
#define IMPOSSIBLE_CALLBACK_HPP

#include <cstdint>
#include <cstring> // memcpy
#include <type_traits>
#include <utility>

namespace ic {
	struct impossible_object;

	namespace detail {
		// The impossible result contains both a function pointer and
		// an object pointer to be called with.
		// The specializations changes the cv qualifiers of the function and object
		template<bool c, bool v, bool e, typename R, typename... Args>
		struct impossible_result;

		template<typename R, typename... Args>
		struct impossible_result<true, true, false, R, Args...> {
			impossible_object const volatile* object;
			auto(*function)(impossible_object const volatile*, Args...) -> R;
		};

		template<typename R, typename... Args>
		struct impossible_result<false, true, false, R, Args...> {
			impossible_object volatile* object;
			auto(*function)(impossible_object volatile*, Args...) -> R;
		};

		template<typename R, typename... Args>
		struct impossible_result<true, false, false, R, Args...> {
			impossible_object const* object;
			auto(*function)(impossible_object const*, Args...) -> R;
		};

		template<typename R, typename... Args>
		struct impossible_result<false, false, false, R, Args...> {
			impossible_object* object;
			auto(*function)(impossible_object*, Args...) -> R;
		};

		template<typename R, typename... Args>
		struct impossible_result<true, true, true, R, Args...> {
			impossible_object const volatile* object;
			auto(*function)(impossible_object const volatile*, Args...) noexcept -> R;
		};

		template<typename R, typename... Args>
		struct impossible_result<false, true, true, R, Args...> {
			impossible_object volatile* object;
			auto(*function)(impossible_object volatile*, Args...) noexcept -> R;
		};

		template<typename R, typename... Args>
		struct impossible_result<true, false, true, R, Args...> {
			impossible_object const* object;
			auto(*function)(impossible_object const*, Args...) noexcept -> R;
		};

		template<typename R, typename... Args>
		struct impossible_result<false, false, true, R, Args...> {
			impossible_object* object;
			auto(*function)(impossible_object*, Args...) noexcept -> R;
		};

		// Contains typeless data of an impossible_result.
		struct impossible_addresses {
			std::uintptr_t object;
			std::uintptr_t function;

			template<bool c, bool v, bool e, typename R, typename... Args>
			operator impossible_result<c, v, e, R, Args...>() const {
				impossible_result<c, v, e, R, Args...> result;
				std::memcpy(std::addressof(result), this, sizeof(impossible_addresses));
				return result;
			}
		};

		#ifdef _MSC_VER
			constexpr auto is_itanium() { return falses; }
		#else
			constexpr auto is_itanium() { return true; }
		#endif

		struct gcc_representation {
			std::uintptr_t function;
			std::uintptr_t offset;
		};

		struct msvc_representation_simple {
			std::uintptr_t function;
		};

		struct msvc_representation_multi_inheritance {
			std::uintptr_t function;
			std::uint32_t offset;
		};

		struct msvc_representation_virtual_inheritance {
			std::uintptr_t function;
			std::uint32_t offset;
			std::uint32_t vtable_index;
		};

		struct msvc_representation_unknown {
			std::uintptr_t function;
			std::uint32_t offset;
			std::uint32_t vtable_index;
			std::uint32_t voffset;
			std::uint32_t padding;
		};

		struct no_representation;

		template<typename T>
		using respresentation_for = std::conditional_t<is_itanium(),
			gcc_representation,
			std::conditional_t<sizeof(T) == sizeof(msvc_representation_simple),
				msvc_representation_simple,
				std::conditional_t<sizeof(T) == sizeof(msvc_representation_multi_inheritance),
					msvc_representation_multi_inheritance,
					std::conditional_t<sizeof(T) == sizeof(msvc_representation_virtual_inheritance),
						msvc_representation_virtual_inheritance,
						std::conditional_t<sizeof(T) == sizeof(msvc_representation_unknown),
							msvc_representation_unknown,
							no_representation
						>
					>
				>
			>
		>;

		using vtable_pointer_t = std::uintptr_t*;

		template<typename T>
		auto get_function_representation(T mfptr) {
			using representation = respresentation_for<T>;
			static_assert(not std::is_same_v<representation, no_representation>, "Unknown representation");

			representation mfptr_rep; // uninitialized
			std::memcpy(std::addressof(mfptr_rep), std::addressof(mfptr), sizeof(T));
			return mfptr_rep;
		}

		template<typename T>
		auto apply_offset(T* object, std::uintptr_t offset) -> std::uintptr_t {
			std::uintptr_t result; // uninitialized
			std::memcpy(std::addressof(result), std::addressof(object), sizeof(object));
			return result + offset;
		}

		inline auto get_from_vtable(std::uintptr_t object, std::uintptr_t offset) -> std::uintptr_t {
			vtable_pointer_t* vtable; // uninitialized
			std::memcpy(std::addressof(vtable), std::addressof(object), sizeof(vtable));
			auto const vtable_pointer = *vtable;
			return vtable_pointer[offset];
		}

		template<typename R, typename O, typename T>
		auto impossible_cast(O* object, T mfptr) -> R {
			auto const mfptr_rep = get_function_representation(mfptr);

			if constexpr (std::is_same_v<decltype(mfptr_rep), gcc_representation const>) {
				if (mfptr_rep.function & 1) {
					// get function from vtable
					auto const ajusted = apply_offset(object, mfptr_rep.offset);
					auto const function = get_from_vtable(ajusted, (mfptr_rep.function - 1) >> 1);
					return impossible_addresses{ajusted, function};
				} else {
					return impossible_addresses{apply_offset(object, mfptr_rep.offset), mfptr_rep.function};
				}
			} else if constexpr (std::is_same_v<decltype(mfptr_rep), msvc_representation_simple const>) {
				return impossible_addresses{object, mfptr_rep.function};
			} else if constexpr (std::is_same_v<decltype(mfptr_rep), msvc_representation_multi_inheritance const>) {
				return impossible_addresses{apply_offset(object, mfptr_rep.offset), mfptr_rep.function};
			} else if constexpr (std::is_same_v<decltype(mfptr_rep), msvc_representation_virtual_inheritance const>) {
				// TODO: implement
				return impossible_addresses{};
			} else if constexpr (std::is_same_v<decltype(mfptr_rep), msvc_representation_unknown const>) {
				// TODO: implement
				return impossible_addresses{};
			} else {
				static_assert(std::is_same_v<T, T>, "Unknown representation");
			}
		}
	} // namespace detail

	template<typename O, typename C, typename R, typename... Args>
	auto impossible_callback(O* object, auto(C::*function)(Args...) -> R) -> detail::impossible_result<false, false, false, R, Args...> {
		return detail::impossible_cast<detail::impossible_result<false, false, false, R, Args...>>(static_cast<C*>(object), function);
	}
} // namespace ic

#endif // IMPOSSIBLE_CALLBACK_HPP
