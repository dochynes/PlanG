CC      ?= gcc
CXX     ?= g++
AR      ?= ar
RM      ?= rm -f
MKDIR   ?= mkdir -p


BUILD_DIR := build
INC_DIRS  := -Iinclude -Ivendor/plantri

PLUGIN_SHIM := $(CURDIR)/src/bridge/plugin_shim.c

CFLAGS   ?= -O2 -std=c11 -Wall -Wextra -Wpedantic
CXXFLAGS ?= -O2 -std=c++20 -Wall -Wextra -Wpedantic
LDFLAGS  ?=


BRIDGE_LIB := $(BUILD_DIR)/libplantri_bridge.a
DEMO_EXE   := $(BUILD_DIR)/plantri_demo


# zdroje
PLANTRI_C := vendor/plantri/plantri.c
BRIDGE_CPP := src/bridge/bridge_api.cpp
MAIN_CPP   := src/main.cpp

.PHONY: all clean run

all: $(DEMO_EXE)



# staticka knihovna s plantri + vlozenym plugin_shim.c
$(BRIDGE_LIB): $(BUILD_DIR)/plantri_bridge.o
	$(AR) rcs $@ $<


# kompilujeme plantri.c s prejmenovanim main a vlozenim pluginu
$(BUILD_DIR)/plantri_bridge.o: $(PLANTRI_C) $(PLUGIN_SHIM)
	$(MKDIR) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC_DIRS) \
	  -Dmain=plantri_run \
	  -DPLUGIN=\"$(PLUGIN_SHIM)\" \
	  -Wno-unused-parameter \
	  -Wno-sign-compare \
	  -Wno-maybe-uninitialized \
	  -c $(PLANTRI_C) -o $@

#  C++ glue objekt
$(BUILD_DIR)/bridge_api.o: $(BRIDGE_CPP) include/plantri/BridgeAPI.hpp
	$(MKDIR) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $(BRIDGE_CPP) -o $@


#  main objekt
$(BUILD_DIR)/main.o: $(MAIN_CPP) include/plantri/BridgeAPI.hpp
	$(MKDIR) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $(MAIN_CPP) -o $@

#  Link
$(DEMO_EXE): $(BUILD_DIR)/main.o $(BUILD_DIR)/bridge_api.o $(BRIDGE_LIB)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)


run: $(DEMO_EXE)
	./$(DEMO_EXE)


clean:
	$(RM) -r $(BUILD_DIR)