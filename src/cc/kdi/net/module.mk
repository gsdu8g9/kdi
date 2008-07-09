MAGIC_MODULE_DEPS := kdi_tablet
MAGIC_EXTERNAL_DEPS := Ice IceUtil
MAGIC_LINK_TYPE := shared
include magic.mk

# Manually add dependencies on generated headers
$(dep)/kdi/net/TableManagerI.d $(BUILD_DIR)/TableManagerI.o: $(BUILD_DIR)/TableManager.h
$(dep)/kdi/net/TimeoutLocator.d $(BUILD_DIR)/TimeoutLocator.o: $(BUILD_DIR)/TableManager.h
$(dep)/kdi/net/net_table.d $(BUILD_DIR)/net_table.o: $(BUILD_DIR)/TableManager.h