PLUGIN_NAME = sandbox_plugin
GCC_PLUGIN_DIR := $(shell gcc -print-file-name=plugin)

CFLAGS = -fPIC -shared -I$(GCC_PLUGIN_DIR)/include

SRC = plugin/sandbox_plugin.c \
      plugin/sandbox_pass.c \
      plugin/detection_engine.c \
      plugin/policy_engine.c \
      plugin/enforcement.c \
      plugin/logger.c

all:
	gcc $(CFLAGS) $(SRC) -o $(PLUGIN_NAME).so

clean:
	rm -f *.so
