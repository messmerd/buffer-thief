# Buffer Thief

A header-only C++ library for safely stealing the internal heap-allocated buffers of the Standard Library's strings and vectors.

Useful for transferring ownership of a string or vector across a C API (potentially for interop with another programming language) without incurring unnecessary heap allocations or copies.

## API reference

### `std::basic_string<CharT>`
```cpp
// <bufferthief/string.hh>

//! @returns internal buffer of string, or nullptr if the small string optimization (SSO) is used
template<typename CharT>
auto try_steal(std::basic_string<CharT>& input) noexcept -> std::unique_ptr<CharT[]>;

//! @returns string contents, stealing internal buffer when possible and copying if not
template<typename CharT>
auto steal(std::basic_string<CharT>&& input) -> std::unique_ptr<CharT[]>;

//! @returns whether the string uses a large buffer, indicating the buffer can be stolen
template<typename CharT>
auto uses_large_buffer(const std::basic_string<CharT>& input) noexcept -> bool;

//! @returns the maximum size of the small string buffer in characters, not including the null terminator
template<typename CharT>
constexpr auto small_string_max_size() noexcept -> std::size_t;
```
> [!NOTE]
> `uses_large_buffer()` is constexpr in C++20, while `try_steal()` and `steal()` are `constexpr` in C++23.

### `std::vector<T>`
```cpp
// <bufferthief/vector.hh>

//! @returns internal buffer of the vector or nullptr if empty
template<typename T>
auto steal(std::vector<T>&& input) noexcept -> std::unique_ptr<T[]>;
```
> [!NOTE]
> `steal()` is constexpr in C++23, and uses `noexcept(false)` when `BT_COPY_BUFFERS` is defined.

## Build

Linux and macOS:

```bash
cmake -B build
cmake --build build
```

Windows (MSVC):

```bash
cmake -B build
cmake --build build --config Release
```

## Configuration options

The following CMake options are available when configuring:

```
BT_COPY_BUFFERS (default: OFF)
```
When defined, all functions that would normally steal an internal buffer instead copy it - if the function's semantics allow. So `steal()` will always copy the internal buffer, and `try_steal()` will always return `nullptr`. Any other function which requires access to a container or string's implementation details is not provided.

**Purpose:** Comparing performance of stealing buffers vs copying buffers; Allowing code using Buffer Thief to compile even when using an unsupported standard library implementation

```
BT_BUILD_TESTS (default: OFF, unless top-level project)
```
Builds Buffer Thief's unit tests.

## Install

After building, install to your system to begin using in projects:
```bash
cmake --install build
```

Or on Windows with MSVC:
```bash
cmake --install build --config Release
```

## CMake usage

After installing, you can import Buffer Thief as a dependency in any C++ project that uses CMake:

```cmake
find_package(bufferthief CONFIG REQUIRED)

add_executable(MyProject main.cpp)
target_link_libraries(MyProject PRIVATE messmerd::bufferthief)
```

## C++ example

```cpp
#include <bufferthief/string.hh>
#include <fstream>

// C++ library
struct Document
{
public:
	explicit Document(const char* file_path)
		: fin_{file_path}
	{}

	auto read_line() -> std::string {
		std::string line;
		std::getline(fin_, line);
		return line;
  }

private:
	std::ifstream fin_;
};

// C API for FFI
extern "C" {

Document* document_new(const char* file_path) { return new Document{file_path}; }
void      document_delete(Document* context) { delete context; }

#if 1

// with Buffer Thief
char* document_read_line(Document* context) {
	auto str = context->read_line();

	// Steals the string's internal heap-allocated buffer if it has one,
	// otherwise allocates a copy of the small string on the heap
	return bt::steal(std::move(str)).release();
}

#else

// without Buffer thief
char* document_read_line(Document* context) {
	auto str = context->read_line();

	// Always need to copy the string
	char* copy = new char[str.size() + 1];
	std::char_traits<char>::copy(copy, str.c_str(), str.size());
	copy[str.size()] = '\0';

	return copy;
}
#endif

void cstring_delete(char* p) { delete[] p; }

}
```

## Requirements

- libstdc++ (GCC), libc++ (Clang), or STL (MSVC)
- C++17 or higher

## TODO

- Buffer thief implementations for `std::vector`
- Polymorphic allocator support
- ASAN compatibility
