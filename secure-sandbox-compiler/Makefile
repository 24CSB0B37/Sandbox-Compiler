PLUGIN_FLAGS = -I$(shell gcc -print-file-name=plugin)/include
PLUGIN_SRC   = plugin/detection_engine.cc plugin/policy_engine.cc \
               plugin/enforcement.cc plugin/logger.cc \
               plugin/sandbox_pass.cc plugin/sandbox_plugin.cc

all: plugin build_runtime

sandbox_plugin.so: $(PLUGIN_SRC) plugin/*.h
	g++ -shared -fPIC -fno-rtti $(PLUGIN_FLAGS) $(PLUGIN_SRC) -o sandbox_plugin.so

plugin: sandbox_plugin.so

build_runtime: runtime/sandbox_wrappers.o

test_system: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/system_violation.c \
	    runtime/sandbox_wrappers.o -o out

test_malloc: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/malloc_large.c \
	    runtime/sandbox_wrappers.o -o out

test_pointer: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/pointer_arith.c \
	    runtime/sandbox_wrappers.o -o out

test_safe: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/safe.c \
	    runtime/sandbox_wrappers.o -o out

test_exec: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/exec_usage.c \
	    runtime/sandbox_wrappers.o -o out

test_fork: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/fork_usage.c \
	    runtime/sandbox_wrappers.o -o out

test_overflow_pattern: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/overflow_pattern.c \
	    runtime/sandbox_wrappers.o -o out

test_socket: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/socket_usage.c \
	    runtime/sandbox_wrappers.o -o out

test_fopen: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/fopen_usage.c \
	    runtime/sandbox_wrappers.o -o out

test_fwrite: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/fwrite_usage.c \
	    runtime/sandbox_wrappers.o -o out

test_inline_assembly: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/inline_assembly.c \
	    runtime/sandbox_wrappers.o -o out

test_hardcoded_path: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/hardcoded_path.c \
	    runtime/sandbox_wrappers.o -o out

test_thread_creation: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/thread_creation.c \
	    runtime/sandbox_wrappers.o -o out

test_memory_leak: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/memory_leak.c \
	    runtime/sandbox_wrappers.o -o out

test_recursion: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/recursion_depth.c \
	    runtime/sandbox_wrappers.o -o out

test_unsafe_cast: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/unsafe_cast.c \
	    runtime/sandbox_wrappers.o -o out

test_global_variable: sandbox_plugin.so runtime/sandbox_wrappers.o
	-gcc -fplugin=./sandbox_plugin.so test_inputs/global_variable_usage.c \
	    runtime/sandbox_wrappers.o -o out

test_all: test_system test_malloc test_pointer test_safe test_exec test_fork \
	 test_overflow_pattern test_socket test_fopen test_fwrite test_inline_assembly \
	 test_hardcoded_path test_thread_creation test_memory_leak test_recursion \
	 test_unsafe_cast test_global_variable

runtime/sandbox_wrappers.o: runtime/sandbox_wrappers.c
	gcc -c runtime/sandbox_wrappers.c -o runtime/sandbox_wrappers.o

clean:
	rm -f sandbox_plugin.so out runtime/sandbox_wrappers.o