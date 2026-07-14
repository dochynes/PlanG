CC ?= gcc
CXX ?= g++
RM ?= rm -f
MKDIR ?= mkdir -p

UNAME_S := $(shell uname -s)

LIB_DIR := lib
SRC_GENG_DIR := src/geng
SRC_PLANTRI_DIR := src/plantri

INC_FLAGS := -Iinclude -Ivendor/ -Ivendor/nauty/nauty2_9_1
ifneq ($(BOOST_DIR),)
INC_FLAGS += -I$(BOOST_DIR)
endif

ifeq ($(UNAME_S),Darwin)
APP_FLAGS ?= -O2 -std=c++20 -Wall
else
APP_FLAGS ?= -O3 -flto -march=native -std=c++20 -Wall
endif

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
	@echo "--- Building App ---"
	$(CXX) $(APP_FLAGS) $(INC_FLAGS) \
	$< -L$(LIB_DIR) -lgeng_wrapper -lplantri_wrapper -o $@


clean:
	@echo "--- Cleaning ---"
	$(MAKE) -C $(SRC_GENG_DIR) clean
	$(MAKE) -C $(SRC_PLANTRI_DIR) clean
	$(RM) -r $(LIB_DIR)
	$(RM) $(DEMO_EXE)
