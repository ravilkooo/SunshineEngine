#pragma once

#include <limits>
#include <utility>
#include <string>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <format>
#include <memory>
#include <bitset>
#include <atomic>

namespace vsty {

	template<typename T>
	concept Hashable = requires(T a) {
		{ std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
	};

	/**
	* \brief General strong type
	*
	* T...the type
	* P...phantom type as unique ID (can use __COUNTER__ or vsty::counter<>)
	* D...default m_value (=null m_value), e.g. std::integral_constant<T, 0>
	*/
    template<typename T, auto P, typename D = void >
    struct strong_type_t {
		using value_type = T;
		using phantom_type = decltype(P);
		using default_type = D;

		template <typename U, auto Q, typename R>
		struct is_strong_type : public std::false_type {};

		template <typename U, auto S, typename V, auto Q, typename R>
		struct is_strong_type<strong_type_t<U, S, V>, Q, R> : public std::true_type {};

        strong_type_t() noexcept requires (std::is_same_v<D, void>) = default;					//default constructible
        strong_type_t() noexcept requires (!std::is_same_v<D, void>) { m_value = D::value; };	//explicit from a NULL value
        explicit strong_type_t(const T& val) noexcept { m_value = val; };	//explicit from type T
        explicit strong_type_t(T&& val) noexcept { m_value = std::forward<T>(val); };	//explicit from type T

		template<typename U, typename V>
        explicit strong_type_t(U val1, V val2, size_t number_bits1) noexcept requires std::unsigned_integral<T> { 
			set_bits(val1, 0ull, number_bits1); 
			set_bits(val2, number_bits1);
		}

        strong_type_t( strong_type_t<T, P, D> const &) noexcept = default;		//copy constructible
        strong_type_t( strong_type_t<T, P, D>&&) noexcept = default;			//move constructible

        strong_type_t<T, P, D>& operator=(T const& v) noexcept { m_value = v; return *this; };		//copy assignable from type T
        strong_type_t<T, P, D>& operator=(T&& v) noexcept { m_value = std::forward<T>(v); return *this; };	//copy assignable from type T

        strong_type_t<T, P, D>& operator=(strong_type_t<T, P, D> const&) noexcept = default;	//move assignable
        strong_type_t<T, P, D>& operator=(strong_type_t<T, P, D>&&) noexcept = default;			//move assignable

		T& value() noexcept { return m_value; }	//get reference to the value

		operator const T& () const noexcept { return m_value; }	//retrieve m_value
		operator T& () noexcept { return m_value; }				//retrieve m_value

		T& operator()() noexcept { return m_value; }	//retrieve m_value
		T const & operator()() const noexcept { return m_value; }	//retrieve m_value

		auto operator<=>(const strong_type_t<T, P, D>& v) const = default;
	
		struct equal_to {
			constexpr bool operator()(const T& lhs, const T& rhs) const noexcept requires std::equality_comparable<std::decay_t<T>> { return lhs == rhs; };
		};
		
        struct hash {
            std::size_t operator()(const strong_type_t<T, P, D>& tag) const noexcept requires Hashable<std::decay_t<T>> { return std::hash<T>()(tag.m_value); };
        };

		bool has_value() const noexcept requires ((!is_strong_type<T,P,D>::value) && (!std::is_same_v<D, void>)) { return m_value != D::value; }
		bool has_value() const noexcept requires (is_strong_type<T,P,D>::value) { return m_value.has_value(); }

		//-----------------------------------------------------------------------------------
		// interface mimicking atomic operations - can be used if you selectively want to mimick atomic operations
		// but don't want to use atomic types

		T load() noexcept { return m_value; }	//get the value
		void store( const T& value ) noexcept { m_value = value;  }	//store the value
		void store( T&& value ) noexcept { m_value = std::forward<T>(value);  }	//store the value
		bool compare_exchange_weak( T& expected, T desired ) {
			if( m_value != expected) return false; //might turn this into an assert
			m_value = desired;
			return true;
		}

		bool compare_exchange_strong( T& expected, T desired ) { return compare_exchange_weak(expected, desired); }

		//-----------------------------------------------------------------------------------

		template<typename U>
		auto set_bits(U val, size_t first_bit, const size_t number_bits) requires std::unsigned_integral<T> {
			T value = static_cast<T>(val);
			uint32_t nbits = sizeof(T) * 8;
			assert(first_bit + number_bits <= nbits);
			if( number_bits >= nbits) { m_value = value; return T{}; }

			T umask = first_bit + number_bits < nbits ? static_cast<T>(~0ull) << (first_bit + number_bits) : 0;
			T lmask = first_bit > 0ull ? (1ull << first_bit) - 1 : 0ull;			

			//std::cout << "\nnew value          = " << std::setfill('0') << std::setw(16) << std::hex << std::bitset<64>{value} << std::dec << " first_bit = " << first_bit << " number_bits = " << number_bits <<  std::endl;
			//std::cout << "  m_value          = " << std::setfill('0') << std::setw(16) << std::hex << std::bitset<64>{m_value}  << std::endl;
			//std::cout << "    umask          = " << std::setfill('0') << std::setw(16) << std::hex << std::bitset<64>{umask} << std::endl;
			//std::cout << "    lmask          = " << std::setfill('0') << std::setw(16) << std::hex << std::bitset<64>{lmask} << std::endl;
			//std::cout << "    value          = " << std::setfill('0') << std::setw(16) << std::hex << std::bitset<64>{value} << std::endl;
			//std::cout << "value << first_bit = " << std::setfill('0') << std::setw(16) << std::hex << std::bitset<64>{(value << first_bit)} << std::endl;

			m_value = (m_value & (umask | lmask)) | ((value << first_bit) & ~umask & ~lmask);
			//std::cout << "  new m_value      = " << std::setfill('0') << std::setw(16) << std::hex << std::bitset<64>{m_value}  << std::endl;
			return m_value;
			//std::cout << "  m_value          = " << std::setfill('0') << std::setw(16) << std::hex << m_value << std::endl;
		}

		template<typename U>
		auto set_bits(U value, size_t first_bit) requires std::unsigned_integral<T> {
			return set_bits(value, first_bit, sizeof(T) * 8ull - first_bit);
		}

		auto get_bits(size_t first_bit, size_t number_bits) const noexcept -> T requires std::unsigned_integral<T>  {
			uint32_t nbits = sizeof(T) * 8;
			assert(first_bit < nbits && first_bit + number_bits <= nbits);
			if( number_bits == nbits) return m_value;
			auto val = (m_value >> first_bit) & ((1ull << number_bits) - 1);
			return val;
		}

		auto get_bits(size_t first_bit) const noexcept -> T requires std::unsigned_integral<T>  {
			return get_bits(first_bit, sizeof(T) * 8ull - first_bit);
		}

		auto get_bits_signed(size_t first_bit, size_t number_bits) const noexcept -> T requires std::unsigned_integral<T>  {
			auto value = get_bits(first_bit, number_bits);
			if( value & (1ull << (number_bits - 1))) {
				value |= static_cast<T>(~0ull) << number_bits;
			}
			return value;
		}

		auto get_bits_signed(size_t first_bit) const noexcept -> T requires std::unsigned_integral<T>  {
			return get_bits_signed(first_bit, sizeof(T) * 8ull - first_bit);
		}

	protected:
		T m_value;
	};


	
	//--------------------------------------------------------------------------------------------
	//type counter lifted from https://mc-deltat.github.io/articles/stateful-metaprogramming-cpp20

	template<size_t N>
	struct reader { friend auto counted_flag(reader<N>); };

	template<size_t N>
	struct setter {
		friend auto counted_flag(reader<N>) {}
		static constexpr size_t n = N;
	};

	template< auto Tag, size_t NextVal = 0 >
	[[nodiscard]] consteval auto counter_impl() {
		constexpr bool counted_past_m_value = requires(reader<NextVal> r) { counted_flag(r); };

		if constexpr (counted_past_m_value) {
			return counter_impl<Tag, NextVal + 1>();
		}
		else {
			setter<NextVal> s;
			return s.n;
		}
	}

	template< auto Tag = [] {}, auto Val = counter_impl<Tag>() >
	constexpr auto counter = Val;

}


