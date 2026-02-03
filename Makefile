CC ?= gcc
CXX ?= g++
RM ?= rm -f
MKDIR ?= mkdir -p

LIB_DIR := lib
SRC_GENG_DIR := src/geng
SRC_PLANTRI_DIR := src/plantri

INC_FLAGS := -Iinclude -Ivendor/ -Ivendor/nauty/nauty2_9_1

GENG_LIB_NAME := libgeng_wrapper.a
GENG_LIB_PATH := $(SRC_GENG_DIR)/$(GENG_LIB_NAME)

PLANTRI_LIB_NAME := libplantri_wrapper.a
PLANTRI_LIB_PATH := $(SRC_PLANTRI_DIR)/$(PLANTRI_LIB_NAME)

DEMO_SRC := main.cpp
DEMO_EXE := plang

all: libs demo

libs: geng_lib plantri_lib
	@$(MKDIR) $(LIB_DIR)
	@echo "--- Copying libraries to $(LIB_DIR) ---"
	cp $(GENG_LIB_PATH) $(LIB_DIR)/
	cp $(PLANTRI_LIB_PATH) $(LIB_DIR)/

geng_lib:
	$(MAKE) -C $(SRC_GENG_DIR)

plantri_lib:                          
	$(MAKE) -C $(SRC_PLANTRI_DIR)

demo: $(DEMO_EXE)

$(DEMO_EXE): $(DEMO_SRC) $(LIB_DIR)/$(GENG_LIB_NAME) $(LIB_DIR)/$(PLANTRI_LIB_NAME)
	@echo "--- Building Demo App ---"
	$(CXX) -O2 -std=c++20 -Wall $(INC_FLAGS) \
	$< -L$(LIB_DIR) -lgeng_wrapper -lplantri_wrapper -o $@


clean:
	@echo "--- Cleaning ---"
	$(MAKE) -C $(SRC_GENG_DIR) clean
	$(MAKE) -C $(SRC_PLANTRI_DIR) clean
	$(RM) -r $(LIB_DIR)
	$(RM) $(DEMO_EXE)