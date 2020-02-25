LINUX_BUILD=$(FLUTTER_ROOT)/packages/flutter_tools/bin/tool_backend.sh
FLUTTER_EPHEMERAL_DIR=flutter/ephemeral

# Configuration provided via flutter tool.
FLUTTER_CONFIG_FILE=$(FLUTTER_EPHEMERAL_DIR)/generated_config.mk
include $(FLUTTER_CONFIG_FILE)

gtk_flutter_test:
	gcc -g -Wall -o gtk_flutter_test main.c fl-view.c `pkg-config --cflags --libs gtk+-3.0`

all: gtk_flutter_test
	$(LINUX_BUILD) linux-x64 release
