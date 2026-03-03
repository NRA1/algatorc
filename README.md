# ALGatorC
C and C++ language module for the ALGator system.

## Usage
- When executing a C++ algorithm
`algatorcpp <project_name> <algorithm_name> <io_filename_root> <times_to_execute> [-dr <algator_data_root_path>] [-dl <algator_data_local_path>]`
- or when executing C algorithm `algatorc <project_name> <algorithm_name> <io_filename_root> <times_to_execute> [-dr <algator_data_root_path>] [-dl <algator_data_local_path>]`

If `<algator_data_root_path>` is not passed, it has a default value of `$ALGATOR_ROOT/data_root`
If `<algator_data_local_path>` is not passed, it has a default value of `$ALGATOR_ROOT/data_local`.

The calls also accept two flags:
- `-c` to recompile the project and algorithm even if source files have not changed
- `-d` to deserialize a new instance of `input` before each call to `execute`

Path `<algator_data_root_path>/projects` must be a valid ALGator project directory and must contain folder `PROJ-<project_name>` with contents:
- when calling `algatorc`
  - `proj/c-src/`
    - `input.c` with `struct input`
    - `output.c` with `struct output`
    - `data_converter.c` with
      - function `struct input* deserialize_input(const char*, unsigned int)` which receives content of `<io_filename_root>.input` and its length and deserializes it into an instance of struct `input`
      - function `char* serialize_output(struct output*, unsigned int*)`, which receives an instance of struct `output`, returns its serialized representation and its representation's length (in bytes).
  - `algs/ALG-<algorithm_name>/src/algorithm.c` which contains algorithm entry function `struct output* execute(struct input*)`
- when calling `algatorcpp`
  - `proj/cpp-src/`
    - `input.cpp` with `class input`
    - `output.cpp` with `class output`
    - `data_converter.cpp` with
      - one of
        - function `input* deserialize_input(const char*, unsigned int)` which receives content of `<io_filename_root>.input` and its length and deserializes it into an instance of class `input`
        - function `input* deserialize_input(std::istream&)` which receives strem of content of `<io_filename_root>.input` and deserializes it into an instance of class `input`
      - and one of
        - function `char* serialize_output(output*, unsigned int*)`, which receives an instance of class `output`, returns its serialized representation and its representation's length (in bytes).
        - function `void serialize_output(std::ostream&, output*)`, which receives an output stream and an instance of class `output`, and writes its serialized representation to the stream.
- `algs/ALG-<algorithm_name>/src/algorithm.cpp` which contains algorithm entry function `output* execute(input*)`

AlgatorC outputs
- file `<io_filename_root>.output` with serialized representation of last `output` instance
- file `<io_filename_root>.status` with
  - in case of a successful execution
    - `OK`
    - `<times_to_execute>` lines with measured execution times of calls to `execute`
  - on case of a failure
    - `ERROR`
    - `type: USER/SYSTEM`
    - `phase: PREPARATION/COMPILATION/SETUP/EXECUTION/TEARDOWN`
    - an error message

## Dependencies
- LLVM 21.1
- Clang 21.1
- libClang 21.1

Additionally on Windows
- dlfcn-win32

### Build dependencies
- CMake
- Ruby

Linux: gcc \
Windows: mingw64

## Building
### Ubuntu
```
git clone https://github.com/NRA1/algatorc.git
cd algatorc
mkdir build
cd build
apt install build-essential cmake clang-21 libclang-21-dev ruby
ln -s /usr/lib/llvm-21/include/clang /usr/local/include/clang
ln -s /usr/lib/llvm-21/include/llvm /usr/local/include/llvm
ln -s /usr/lib/llvm-21/include/llvm-c /usr/local/include/llvm-c
ln -s /usr/lib/llvm-21/include/clang-c /usr/local/include/clang-c
cmake ..
make
```

## Platform support
AlgatorC is primarily intended to run on Linux. \
Preferred way to run AlgatorC on Windows is through WSL (Windows Subsystem for Linux).
While basic native Windows support is provided, memory sandbox and termination interceptor are not currently not
supported, which means that memory, allocated in `execute` is not freed between calls and a call to `exit()` will kill
AlgatorC.

## A note on protections
AlgatorC by design executes arbitrary untrusted user code in-process. While certain protections against common mistakes
in user code are provided to ease algorithm development, a malicious user would have no problem circumventing those
protections and might make AlgatorC misbehave. AlgatorC also does not limit operations that can be performed by the
algorithm, therefore alowing full access to the system with AlgatorC's permissions.

## License
AlgatorC is provided under the terms of GPLv3.