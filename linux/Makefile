LINUX_BUILD=$(FLUTTER_ROOT)/packages/flutter_tools/bin/tool_backend.sh
FLUTTER_EPHEMERAL_DIR=flutter/ephemeral

# Configuration provided via flutter tool.
FLUTTER_CONFIG_FILE=$(FLUTTER_EPHEMERAL_DIR)/generated_config.mk
include $(FLUTTER_CONFIG_FILE)

SOURCES = main.c fl-view.c

gtk_flutter_test: $(SOURCES)
	gcc -g -Wall -o gtk_flutter_test $(SOURCES) -L. -lflutter_engine `pkg-config --cflags --libs gtk+-3.0`

all: gtk_flutter_test
	$(LINUX_BUILD) linux-x64 release
