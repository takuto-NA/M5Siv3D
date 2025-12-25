/**
 * @file SafeKiboSTL.hpp
 * @brief Exception-free and bounds-safe wrappers for Standard Template Library (STL) containers.
 *
 * @details
 * This library provides a "Safe Kibo Standard" (kstd) namespace containing wrappers for
 * commonly used STL components (string, vector, unordered_map, function, optional, array).
 *
 * Key Objectives:
 * 1. Crash Prevention: Microcontrollers typically do not handle C++ exceptions well,
 *    often resulting in a silent hang or a hard reset. These wrappers catch potential
 *    exception-triggering scenarios (like out-of-bounds access) before they happen.
 * 2. Safe Defaults: Instead of throwing exceptions, methods like at() or operator[]
 *    return a static "dummy" default-constructed value when an invalid access is detected.
 * 3. Familiar API: Designed as a nearly drop-in replacement for std:: types to maintain
 *    developer productivity while increasing system reliability.
 *
 * IMPORTANT SAFETY NOTES:
 * - Slicing: Passing these objects by value to functions accepting std:: types will
 *   "slice" the object, losing the safe wrappers and reverting to standard behavior.
 * - Non-virtual Destructors: STL containers are not designed for inheritance.
 *   Avoid polymorphic deletion (e.g., delete std_ptr_to_kstd_obj).
 *   This library is intended for stack-based or direct member usage.
 * - Dummy State: Non-const out-of-bounds access returns a reference to a static
 *   dummy. While we reset this dummy to its default state on each out-of-bounds
 *   access to prevent state leakage, avoid relying on the value of out-of-bounds references.
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <unordered_map>
#include <functional>
#include <optional>
#include <cstdlib>
#include <cstring>
#include <charconv>
#include <system_error>
#include <type_traits>
#include <utility>

#ifdef ARDUINO
    #include <Arduino.h>
#endif

namespace kstd {

    /**
     * @brief Alias for std::string_view.
     *
     * std::string_view is already exception-free and non-owning.
     * We provide this alias for API consistency alongside other kstd::* types.
     */
    using string_view = std::string_view;

    /**
     * @brief Alias for std::nullopt.
     */
    inline constexpr std::nullopt_t nullopt = std::nullopt;

    /**
     * @brief A safe wrapper for std::function.
     * Prevents crashes on bad_function_call by checking if the function is valid before execution.
     */
    template<typename T>
    class function;

    template<typename R, typename... Args>
    class function<R(Args...)> : public std::function<R(Args...)> {
    public:
        using std::function<R(Args...)>::function;

        // Explicit copy and move constructors
        function(const std::function<R(Args...)>& f) : std::function<R(Args...)>(f) {}
        function(std::function<R(Args...)>&& f) : std::function<R(Args...)>(std::move(f)) {}
        function(const function& other) = default;
        function(function&& other) noexcept = default;
        function& operator=(const function& other) = default;
        function& operator=(function&& other) noexcept = default;

        /**
         * @brief Safe call operator. Returns default R if the function is empty.
         */
        R operator()(Args... args) const {
            if (*this) {
                return std::function<R(Args...)>::operator()(std::forward<Args>(args)...);
            }
            if constexpr (!std::is_void_v<R>) {
                return R{};
            }
        }
    };

    /**
     * @brief A safe wrapper for std::optional.
     * Provides an exception-free way to access values.
     */
    template<typename T>
    class optional : public std::optional<T> {
    public:
        using std::optional<T>::optional;

        // Explicit copy and move constructors
        optional(const std::optional<T>& o) : std::optional<T>(o) {}
        optional(std::optional<T>&& o) : std::optional<T>(std::move(o)) {}
        optional(const optional& other) = default;
        optional(optional&& other) noexcept = default;
        optional& operator=(const optional& other) = default;
        optional& operator=(optional&& other) noexcept = default;

        /**
         * @brief Safe value access. Returns a dummy T if the optional is empty.
         * Note: std::optional::value() throws bad_optional_access.
         */
        [[nodiscard]] const T& value() const {
            if (this->has_value()) {
                return std::optional<T>::value();
            }
            static const T dummy{};
            return dummy;
        }

        [[nodiscard]] T& value() {
            if (this->has_value()) {
                return std::optional<T>::value();
            }
            static T dummy{};
            dummy = T{}; // Reset dummy to prevent state leakage
            return dummy;
        }

        /**
         * @brief Explicitly safe access with a default value.
         */
        [[nodiscard]] T value_or_default() const {
            return this->value_or(T{});
        }
    };

    /**
     * @brief A safe wrapper for std::string.
     * Prevents crashes on out-of-bounds access by returning a null character.
     */
    class string : public std::string {
    public:
        using std::string::string;

        // Explicit copy and move constructors
        string(const std::string& s) : std::string(s) {}
        string(std::string&& s) : std::string(std::move(s)) {}
        string(const string& other) = default;
        string(string&& other) noexcept = default;
        string& operator=(const string& other) = default;
        string& operator=(string&& other) noexcept = default;

        /**
         * @brief Safe index operator. Returns a null character if pos is out of bounds.
         */
        [[nodiscard]] const char& operator[](size_t pos) const {
            if (pos >= size()) {
                static const char dummy = '\0';
                return dummy;
            }
            return std::string::operator[](pos);
        }

        /**
         * @brief Safe index operator. Returns a dummy character if pos is out of bounds.
         */
        [[nodiscard]] char& operator[](size_t pos) {
            if (pos >= size()) {
                static char dummy = '\0';
                dummy = '\0'; // Reset dummy to prevent state leakage
                return dummy;
            }
            return std::string::operator[](pos);
        }

        [[nodiscard]] const char& at(size_t pos) const { return (*this)[pos]; }
        [[nodiscard]] char& at(size_t pos) { return (*this)[pos]; }
    };

    /**
     * @brief A safe wrapper for std::vector<T>.
     * Prevents crashes on out-of-bounds access by returning a default-constructed T.
     */
    template <typename T>
    class vector : public std::vector<T> {
    public:
        using std::vector<T>::vector;

        // Explicit copy and move constructors
        vector(const std::vector<T>& v) : std::vector<T>(v) {}
        vector(std::vector<T>&& v) : std::vector<T>(std::move(v)) {}
        vector(const vector& other) = default;
        vector(vector&& other) noexcept = default;
        vector& operator=(const vector& other) = default;
        vector& operator=(vector&& other) noexcept = default;

        /**
         * @brief Safe index operator. Returns a static dummy T if pos is out of bounds.
         */
        [[nodiscard]] const T& operator[](size_t pos) const {
            if (pos >= this->size()) {
                static const T dummy{};
                return dummy;
            }
            return std::vector<T>::operator[](pos);
        }

        /**
         * @brief Safe index operator. Returns a static dummy T if pos is out of bounds.
         */
        [[nodiscard]] T& operator[](size_t pos) {
            if (pos >= this->size()) {
                static T dummy{};
                dummy = T{}; // Reset dummy to prevent state leakage from previous out-of-bounds writes
                return dummy;
            }
            return std::vector<T>::operator[](pos);
        }

        [[nodiscard]] const T& at(size_t pos) const { return (*this)[pos]; }
        [[nodiscard]] T& at(size_t pos) { return (*this)[pos]; }
    };

    /**
     * @brief A safe wrapper for std::array<T, N>.
     * Prevents undefined behavior on out-of-bounds access by returning a default-constructed T.
     *
     * Note: std::array::at() throws std::out_of_range. This wrapper makes at() safe/exception-free.
     */
    template <typename T, size_t N>
    class array : public std::array<T, N> {
    public:
        /**
         * @brief Safe index operator. Returns a static dummy T if pos is out of bounds.
         */
        [[nodiscard]] const T& operator[](size_t pos) const {
            if (pos >= N) {
                static const T dummy{};
                return dummy;
            }
            return std::array<T, N>::operator[](pos);
        }

        /**
         * @brief Safe index operator. Returns a static dummy T if pos is out of bounds.
         */
        [[nodiscard]] T& operator[](size_t pos) {
            if (pos >= N) {
                static T dummy{};
                dummy = T{}; // Reset dummy to prevent state leakage
                return dummy;
            }
            return std::array<T, N>::operator[](pos);
        }

        [[nodiscard]] const T& at(size_t pos) const { return (*this)[pos]; }
        [[nodiscard]] T& at(size_t pos) { return (*this)[pos]; }
    };

    /**
     * @brief A safe wrapper for std::unordered_map<K, V>.
     * Prevents crashes on missing keys by returning a default-constructed V.
     */
    template <typename K, typename V>
    class unordered_map : public std::unordered_map<K, V> {
    public:
        using std::unordered_map<K, V>::unordered_map;

        // Explicit copy and move constructors
        unordered_map(const std::unordered_map<K, V>& m) : std::unordered_map<K, V>(m) {}
        unordered_map(std::unordered_map<K, V>&& m) : std::unordered_map<K, V>(std::move(m)) {}
        unordered_map(const unordered_map& other) = default;
        unordered_map(unordered_map&& other) noexcept = default;
        unordered_map& operator=(const unordered_map& other) = default;
        unordered_map& operator=(unordered_map&& other) noexcept = default;

        /**
         * @brief Safe at() method. Returns a static dummy V if key is not found.
         */
        [[nodiscard]] const V& at(const K& key) const {
            auto it = this->find(key);
            if (it == this->end()) {
                static const V dummy{};
                return dummy;
            }
            return it->second;
        }

        /**
         * @brief Safe at() method. Returns a static dummy V if key is not found.
         */
        [[nodiscard]] V& at(const K& key) {
            auto it = this->find(key);
            if (it == this->end()) {
                static V dummy{};
                dummy = V{}; // Reset dummy to prevent state leakage
                return dummy;
            }
            return it->second;
        }
    };

    /**
     * @brief Safe conversion from kstd::string to int.
     * Uses strtol to avoid exceptions associated with std::stoi.
     */
    [[nodiscard]] inline int stoi(const kstd::string& str, size_t* pos = nullptr, int base = 10) {
        if (str.empty()) return 0;
        char* end;
        long val = std::strtol(str.c_str(), &end, base);
        if (pos) *pos = static_cast<size_t>(end - str.c_str());
        return static_cast<int>(val);
    }

    /**
     * @brief Wrappers for std::to_string returning kstd::string.
     */
    [[nodiscard]] inline kstd::string to_string(int val) { return kstd::string(std::to_string(val)); }
    [[nodiscard]] inline kstd::string to_string(long val) { return kstd::string(std::to_string(val)); }
    [[nodiscard]] inline kstd::string to_string(long long val) { return kstd::string(std::to_string(val)); }
    [[nodiscard]] inline kstd::string to_string(unsigned val) { return kstd::string(std::to_string(val)); }
    [[nodiscard]] inline kstd::string to_string(unsigned long val) { return kstd::string(std::to_string(val)); }
    [[nodiscard]] inline kstd::string to_string(unsigned long long val) { return kstd::string(std::to_string(val)); }
    [[nodiscard]] inline kstd::string to_string(float val) { return kstd::string(std::to_string(val)); }
    [[nodiscard]] inline kstd::string to_string(double val) { return kstd::string(std::to_string(val)); }
    [[nodiscard]] inline kstd::string to_string(long double val) { return kstd::string(std::to_string(val)); }

    /**
     * @brief Safe conversion from kstd::string_view to float.
     * 
     * @details
     * Since std::from_chars for floating-point might not be available in some
     * embedded toolchains, this provides a safe fallback using std::strtof.
     * It ensures no out-of-bounds access by copying to a small stack buffer
     * if the input is not null-terminated.
     */
    [[nodiscard]] inline kstd::optional<float> parse_float(kstd::string_view sv) {
        if (sv.empty()) return kstd::nullopt;

        // Try to avoid copy if possible, but strtof needs null termination.
        // For safety, we always copy to a small stack buffer.
        char buf[32];
        const size_t len = (sv.size() < sizeof(buf) - 1) ? sv.size() : sizeof(buf) - 1;
        std::memcpy(buf, sv.data(), len);
        buf[len] = '\0';

        char* end;
        float val = std::strtof(buf, &end);
        if (end == buf) return kstd::nullopt;
        return val;
    }

} // namespace kstd

/**
 * @brief Hash specialization for kstd::string to allow usage in std::unordered_map.
 */
namespace std {
    template <>
    struct hash<kstd::string> {
        size_t operator()(const kstd::string& s) const noexcept {
            return std::hash<std::string>{}(s);
        }
    };
}

