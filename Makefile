# Tools
CC    = g++
LD    = g++
FMT   = clang-format

# Directories
SRC_DIR  = src
INC_DIR  = inc
TMP_DIR  = obj
BIN_DIR  = bin
DOX_DIR  = doxygen
C_EXT    = cpp
S_EXT    = S
H_EXT    = h
SUBDIRS  =

# Configuration
PRODUCT        = axolotl
DEFINES        =
CC_FLAGS       = -O0 -g -ggdb
AS_FLAGS       =
LD_FLAGS       =

# Mandatory CC flags
CC_FLAGS += -std=gnu++11
CC_FLAGS += -Wall -Wextra -Wno-unused-function -Wno-unused-parameter
CC_FLAGS +=
CC_FLAGS += $(DEFINES)
CC_FLAGS += -I$(INC_DIR) -I$(SRC_DIR)

# Format flags
FMT_FLAGS = -i -style=file

# Additional macros
define \n


endef

# Sources management
C_SUB  = $(shell find $(SRC_DIR) -type d)
H_SUB  = $(shell find $(INC_DIR) -type d)

C_SRC  = $(wildcard $(addsuffix /*.$(C_EXT),$(C_SUB)))
C_OBJ  = $(patsubst $(SRC_DIR)/%.$(C_EXT),$(TMP_DIR)/%.o,$(C_SRC))
C_DEP  = $(patsubst $(SRC_DIR)/%.$(C_EXT),$(TMP_DIR)/%.d,$(C_SRC))

S_SRC  = $(wildcard $(addsuffix /*.$(S_EXT),$(C_SUB)))
S_OBJ  = $(patsubst $(SRC_DIR)/%.$(S_EXT),$(TMP_DIR)/%.o,$(S_SRC))
S_DEP  = $(patsubst $(SRC_DIR)/%.$(S_EXT),$(TMP_DIR)/%.d,$(S_SRC))

C_FMT  = $(foreach d,$(C_SUB),$(patsubst $(d)/%.$(C_EXT),$(d)/fmt-%,$(wildcard $(d)/*.$(C_EXT))))
H_FMT  = $(foreach d,$(H_SUB),$(patsubst $(d)/%.$(H_EXT),$(d)/fmt-%,$(wildcard $(d)/*.$(H_EXT))))

# Product files
BINARY = $(BIN_DIR)/$(PRODUCT)
DIST   = $(BIN_DIR)/$(PRODUCT).tar.gz

# Top-level
all: binary

binary: subdirs $(BINARY)

.PHONY: doxygen
doxygen:
	@mkdir -p $(DOX_DIR)
	@doxygen Doxyfile

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR) $(TMP_DIR) $(DOX_DIR)
	$(foreach sub,$(SUBDIRS),@$(MAKE) --no-print-directory -C $(sub) clean${\n})

.PHONY: format
format: $(C_FMT) $(H_FMT)

.PHONY: subdirs
subdirs:
	$(foreach sub,$(SUBDIRS),@$(MAKE) --no-print-directory -C $(sub)${\n})

.PHONY: tar
tar: clean $(DIST)

.PHONY: $(DIST)
$(DIST):
	@mkdir -p $(@D)
	@tar -cvzf $@ . --exclude $(@D) > /dev/null

# Special targets

# Dependencies
-include $(C_DEP)

# Translation
$(BINARY): $(C_OBJ) $(S_OBJ)
	@mkdir -p $(@D)
	@echo "(LD)      $@"
	@$(LD) -o $@ $^ $(LD_FLAGS)

$(TMP_DIR)/%.o: $(SRC_DIR)/%.$(C_EXT)
	@mkdir -p $(@D)
	@echo "(CC)      $<"
	@$(CC) $(CC_FLAGS) -MMD -c $< -o $@

$(TMP_DIR)/%.o: $(SRC_DIR)/%.$(S_EXT)
	@mkdir -p $(@D)
	@echo "(CC)      $<"
	@$(CC) $(CC_FLAGS) -MMD -c $< -o $@

# Format
fmt-%: %.$(C_EXT)
	@$(FMT) $(FMT_FLAGS) $<
	@echo "(FMT)     $<"

fmt-%: %.$(H_EXT)
	@$(FMT) $(FMT_FLAGS) $<
	@echo "(FMT)     $<"
