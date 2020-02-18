LINUX_BUILD=$(FLUTTER_ROOT)/packages/flutter_tools/bin/tool_backend.sh
FLUTTER_EPHEMERAL_DIR=flutter/ephemeral

# Configuration provided via flutter tool.
FLUTTER_CONFIG_FILE=$(FLUTTER_EPHEMERAL_DIR)/generated_config.mk
include $(FLUTTER_CONFIG_FILE)

all:
	$(LINUX_BUILD) linux-x64 release
