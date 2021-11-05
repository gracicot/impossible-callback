#ifndef IMPOSSIBLE_CALLBACK_HPP
#define IMPOSSIBLE_CALLBACK_HPP

#include <cstdint>
#include <cstring> // memcpy
#include <memory> // addressof
#include <type_traits>
#include <utility>

namespace impossible_callback {
	struct impossible_object {};

	struct drop_cv_t {} inline constexpr drop_cv{};
	struct drop_cv_noexcept_t {} inline constexpr drop_cv_noexcept{};
	struct enforce_noexcept_t {} inline constexpr enforce_noexcept{};
	struct drop_cv_enforce_noexcept_t {} inline constexpr drop_cv_enforce_noexcept;

	namespace detail {
		constexpr auto is_itanium() -> bool {
			return
				#ifdef _MSC_VER
					false;
				#else
					true;
				#endif
		}

		template<typename>
		struct impossible_function_pointer;

		template<typename R, typename T, typename... Args>
		struct impossible_function_pointer<auto(T*, Args...) -> R> {
			using type = std::conditional_t<is_itanium(),
				auto(*)(T*, Args...) -> R,
				std::conditional_t<std::is_const_v<T>,
					std::conditional_t<std::is_volatile_v<T>,
						auto(Args...) volatile const& -> R,
						auto(Args...) const& -> R
					>,
					std::conditional_t<std::is_volatile_v<T>,
						auto(Args...) volatile& -> R,
						auto(Args...) & -> R
					>
				> impossible_object::*
			>;
		};

		template<typename R, typename T, typename... Args>
		struct impossible_function_pointer<auto(T*, Args...) noexcept -> R> {
			using type = std::conditional_t<is_itanium(),
				auto(*)(T*, Args...)->R,
				std::conditional_t<std::is_const_v<T>,
					std::conditional_t<std::is_volatile_v<T>,
						auto(Args...) volatile const& noexcept -> R,
						auto(Args...) const& noexcept -> R
					>,
					std::conditional_t<std::is_volatile_v<T>,
						auto(Args...) volatile& noexcept -> R,
						auto(Args...) & noexcept -> R
					>
				> impossible_object::*
			>;
		};

		template<typename F>
		using impossible_function_pointer_t = typename impossible_function_pointer<F>::type;

		enum struct qualifier : std::uint8_t {
			no_qualifier =            0b000,
			only_const =              0b001,
			only_volatile =           0b010,
			const_volatile =          only_const | only_volatile,
			only_noexcept =           0b100,
			const_noexcept =          only_const | only_noexcept,
			volatile_noexcept =       only_volatile | only_noexcept,
			const_volatile_noexcept = only_const | only_volatile | only_noexcept
		};

		constexpr auto operator|(qualifier lhs, qualifier rhs) -> qualifier {
			return static_cast<qualifier>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
		}

		constexpr auto operator&(qualifier lhs, qualifier rhs) -> qualifier {
			return static_cast<qualifier>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
		}

		constexpr auto operator^(qualifier lhs, qualifier rhs) -> qualifier {
			return static_cast<qualifier>(static_cast<std::uint8_t>(lhs) ^ static_cast<std::uint8_t>(rhs));
		}

		constexpr auto operator~(qualifier q) -> qualifier {
			return static_cast<qualifier>(~static_cast<std::uint8_t>(q) & 0b111u);
		}

		template<typename...> struct metalist {};

		template<typename T, qualifier q>
		using apply_qualifiers_on = std::conditional_t<(q & qualifier::only_const) != qualifier::no_qualifier,
			std::conditional_t<(q & qualifier::only_volatile) != qualifier::no_qualifier,
				T const volatile,
				T const
			>,
			std::conditional_t<(q & qualifier::only_volatile)!= qualifier::no_qualifier,
				T volatile,
				T
			>
		>;

		template<qualifier q>
		using qualified_impossible_object_type = apply_qualifiers_on<impossible_object, q>;

		template<qualifier q, typename return_type, typename... Args>
		using qualified_impossible_function_type = impossible_function_pointer_t<
			std::conditional_t<(q & qualifier::only_noexcept) != qualifier::no_qualifier,
				auto(qualified_impossible_object_type<q>*, Args...) noexcept -> return_type,
				auto(qualified_impossible_object_type<q>*, Args...) -> return_type
			>
		>;

		template<qualifier q, typename Type, typename R, typename... Args>
		struct base_member_function_traits {
			static constexpr auto qualifiers = q;
			using object_type = apply_qualifiers_on<Type, q>;
			using return_type = R;
			using arguments_type = metalist<Args...>;
		};

		template<typename F>
		struct member_function_traits;

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...)> : base_member_function_traits<qualifier::no_qualifier, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...)&> : base_member_function_traits<qualifier::no_qualifier, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) const> : base_member_function_traits<qualifier::only_const, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) const&> : base_member_function_traits<qualifier::only_const, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) volatile> : base_member_function_traits<qualifier::only_volatile, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) volatile&> : base_member_function_traits<qualifier::only_volatile, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) const volatile> : base_member_function_traits<qualifier::const_volatile, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) const volatile&> : base_member_function_traits<qualifier::const_volatile, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) noexcept> : base_member_function_traits<qualifier::only_noexcept, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) & noexcept> : base_member_function_traits<qualifier::only_noexcept, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) const noexcept> : base_member_function_traits<qualifier::const_noexcept, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) const& noexcept> : base_member_function_traits<qualifier::const_noexcept, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) volatile noexcept> : base_member_function_traits<qualifier::volatile_noexcept, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) volatile& noexcept> : base_member_function_traits<qualifier::volatile_noexcept, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) const volatile noexcept> : base_member_function_traits<qualifier::const_volatile_noexcept, Type, R, Args...> {};

		template <typename Type, typename R, typename... Args>
		struct member_function_traits<R(Type::*)(Args...) const volatile& noexcept> : base_member_function_traits<qualifier::const_volatile_noexcept, Type, R, Args...> {};

		template<typename F>
		using return_type_of = typename member_function_traits<F>::return_type;

		template<typename F>
		using arguments_of = typename member_function_traits<F>::arguments_type;

		template<typename F>
		using object_type_of = typename member_function_traits<F>::object_type;

		template<typename F>
		inline constexpr auto qualifiers_of = member_function_traits<F>::qualifiers;

		// The impossible result contains both a function pointer and
		// an object pointer to be called with.
		// The specializations changes the cv qualifiers of the function and object
		template<qualifier q, typename R, typename Args>
		struct impossible_result;

		template<qualifier q, typename R, typename... Args>
		struct impossible_result<q, R, metalist<Args...>> {
			qualified_impossible_object_type<q>* object;
			qualified_impossible_function_type<q, R, Args...> function;
		};

		template<typename F, qualifier q = qualifiers_of<F>>
		using impossible_result_for = impossible_result<q, return_type_of<F>, arguments_of<F>>;

		// Contains typeless data of an impossible_result.
		struct impossible_addresses {
			std::uintptr_t object;
			std::uintptr_t function;

			template<qualifier q, typename R, typename... Args>
			operator impossible_result<q, R, Args...>() const {
				impossible_result<q, R, Args...> result;
				std::memcpy(std::addressof(result), this, sizeof(impossible_addresses));
				return result;
			}
		};

		struct gcc_representation {
			std::uintptr_t function;
			std::uintptr_t offset;
		};

		struct msvc_representation_simple {
			std::uintptr_t function;
		};

		struct msvc_representation_multi_or_virtual_inheritance {
			std::uintptr_t function;
			std::uint32_t offset;
			std::uint32_t vtable_index;
		};

		struct msvc_representation_unknown {
			std::uintptr_t function;
			std::uint32_t offset;
			std::uint32_t voffset;
			std::uint32_t vindex;
			std::uint32_t padding;
		};

		struct no_representation;

		template<typename T>
		using respresentation_for = std::conditional_t<is_itanium(),
			gcc_representation,
			std::conditional_t<sizeof(T) == sizeof(msvc_representation_simple),
				msvc_representation_simple,
				std::conditional_t<sizeof(T) == sizeof(msvc_representation_multi_or_virtual_inheritance),
					msvc_representation_multi_or_virtual_inheritance,
					std::conditional_t<sizeof(T) == sizeof(msvc_representation_unknown),
						msvc_representation_unknown,
						no_representation
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
			constexpr auto s = sizeof(T);
			std::memcpy(std::addressof(mfptr_rep), std::addressof(mfptr), s);
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
				return impossible_addresses{apply_offset(object, 0), mfptr_rep.function};
			} else if constexpr (std::is_same_v<decltype(mfptr_rep), msvc_representation_multi_or_virtual_inheritance const>) {
				// TODO: Figure out how to recognize virtual inheritance, assume no virtual inheritance for now
				return impossible_addresses{apply_offset(object, mfptr_rep.offset), mfptr_rep.function};
			} else if constexpr (std::is_same_v<decltype(mfptr_rep), msvc_representation_unknown const>) {
				if (mfptr_rep.vindex == 0) {
					return impossible_addresses{apply_offset(object, mfptr_rep.offset), mfptr_rep.function};
				} else {
					return impossible_addresses{apply_offset(object, mfptr_rep.offset + mfptr_rep.voffset + get_from_vtable(apply_offset(object, mfptr_rep.voffset), mfptr_rep.vindex)), mfptr_rep.function};
				}
			} else {
				static_assert(std::is_same_v<T, T>, "Unknown representation");
			}
		}

		template<typename F>
		using enforce_noexcept_condition_t = std::enable_if_t<(qualifiers_of<F> & qualifier::only_noexcept) != qualifier::no_qualifier, enforce_noexcept_t>;

		template<typename F>
		using drop_cv_enforce_noexcept_condition_t = std::enable_if_t<(qualifiers_of<F> & qualifier::only_noexcept) != qualifier::no_qualifier, drop_cv_enforce_noexcept_t>;
	} // namespace detail

	template<typename R, typename... Args>
	using impossible_function = detail::qualified_impossible_function_type<detail::qualifier::no_qualifier, R, Args...>;

	template<typename R, typename... Args>
	using impossible_function_const = detail::qualified_impossible_function_type<detail::qualifier::only_const, R, Args...>;

	template<typename R, typename... Args>
	using impossible_function_volatile = detail::qualified_impossible_function_type<detail::qualifier::only_volatile, R, Args...>;

	template<typename R, typename... Args>
	using impossible_function_noexcept = detail::qualified_impossible_function_type<detail::qualifier::only_noexcept, R, Args...>;

	template<typename R, typename... Args>
	using impossible_function_const_volatile = detail::qualified_impossible_function_type<detail::qualifier::const_volatile, R, Args...>;

	template<typename R, typename... Args>
	using impossible_function_const_noexcept = detail::qualified_impossible_function_type<detail::qualifier::const_noexcept, R, Args...>;

	template<typename R, typename... Args>
	using impossible_function_volatile_noexcept = detail::qualified_impossible_function_type<detail::qualifier::volatile_noexcept, R, Args...>;

	template<typename R, typename... Args>
	using impossible_function_const_volatile_noexcept = detail::qualified_impossible_function_type<detail::qualifier::const_volatile_noexcept, R, Args...>;

	template<typename O, typename F>
	inline auto impossible_callback(O* object, F function) -> detail::impossible_result_for<F> {
		using result_type = detail::impossible_result_for<F>;
		return detail::impossible_cast<result_type>(static_cast<detail::object_type_of<F>*>(object), function);
	}

	template<typename O, typename F>
	inline auto impossible_callback(O* object, F function, drop_cv_t) -> detail::impossible_result_for<F, detail::qualifiers_of<F> & detail::qualifier::only_noexcept> {
		using result_type = detail::impossible_result_for<F, detail::qualifiers_of<F> & detail::qualifier::only_noexcept>;
		return detail::impossible_cast<result_type>(static_cast<detail::object_type_of<F>*>(object), function);
	}

	template<typename O, typename F>
	inline auto impossible_callback(O* object, F function, drop_cv_noexcept_t) -> detail::impossible_result_for<F, detail::qualifier::no_qualifier> {
		using result_type = detail::impossible_result_for<F, detail::qualifier::no_qualifier>;
		return detail::impossible_cast<result_type>(static_cast<detail::object_type_of<F>*>(object), function);
	}

	template<typename O, typename F>
	inline auto impossible_callback(O* object, F function, detail::enforce_noexcept_condition_t<F>) -> detail::impossible_result_for<F> {
		using result_type = detail::impossible_result_for<F>;
		return detail::impossible_cast<result_type>(static_cast<detail::object_type_of<F>*>(object), function);
	}

	template<typename O, typename F>
	inline auto impossible_callback(O* object, F function, detail::drop_cv_enforce_noexcept_condition_t<F>) -> detail::impossible_result_for<F, detail::qualifier::only_noexcept> {
		using result_type = detail::impossible_result_for<F, detail::qualifier::only_noexcept>;
		return detail::impossible_cast<result_type>(static_cast<detail::object_type_of<F>*>(object), function);
	}

} // namespace impossible_callback

#endif // IMPOSSIBLE_CALLBACK_HPP
