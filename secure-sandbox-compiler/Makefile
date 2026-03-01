PLUGIN_NAME = sandbox_plugin
GCC_PLUGIN_DIR := $(shell gcc -print-file-name=plugin)

CXX = g++
CXXFLAGS = -fPIC -shared -std=gnu++17 \
           -I$(GCC_PLUGIN_DIR)/include \
           -fno-rtti -fno-exceptions

SRC = plugin/sandbox_plugin.cc \
      plugin/sandbox_pass.cc \
      plugin/detection_engine.cc \
      plugin/policy_engine.cc \
      plugin/enforcement.cc \
      plugin/logger.cc

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(PLUGIN_NAME).so

clean:
	rm -f *.so