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
	gcc -fplugin=./sandbox_plugin.so test_inputs/system_violation.c \
	    runtime/sandbox_wrappers.o -o out

test_malloc: sandbox_plugin.so runtime/sandbox_wrappers.o
	gcc -fplugin=./sandbox_plugin.so test_inputs/malloc_large.c \
	    runtime/sandbox_wrappers.o -o out

test_pointer: sandbox_plugin.so runtime/sandbox_wrappers.o
	gcc -fplugin=./sandbox_plugin.so test_inputs/pointer_arith.c \
	    runtime/sandbox_wrappers.o -o out

test_safe: sandbox_plugin.so runtime/sandbox_wrappers.o
	gcc -fplugin=./sandbox_plugin.so test_inputs/safe.c \
	    runtime/sandbox_wrappers.o -o out

runtime/sandbox_wrappers.o: runtime/sandbox_wrappers.c
	gcc -c runtime/sandbox_wrappers.c -o runtime/sandbox_wrappers.o

clean:
	rm -f sandbox_plugin.so out runtime/sandbox_wrappers.o