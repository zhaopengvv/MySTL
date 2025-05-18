# MySTL

A C++ STL implementation for learning purposes.

## Building and Testing

### Prerequisites
- CMake 3.14 or higher
- C++14 compatible compiler
- Git

### Initial Setup

1. Clone the repository with submodules:
```bash
git clone --recursive https://github.com/yourusername/MySTL.git
```

Or if you've already cloned the repository:
```bash
git submodule init
git submodule update
```

### Build Instructions

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure the project:
```bash
cmake ..
```

3. Build the project:
```bash
cmake --build .
```

4. Run the tests:
```bash
ctest --output-on-failure
```

or run the test executable directly:
```bash
./tests/mystl_test
```

## Project Structure

- `construct.h`: Implementation of object construction and destruction utilities
- `allocator.h`: Custom allocator implementation
- `iterator.h`: Iterator traits and utilities
- `tests/`: Unit tests using Google Test framework
- `third_party/`: Third-party dependencies
  - `googletest/`: Google Test framework (as git submodule) 