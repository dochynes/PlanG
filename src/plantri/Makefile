ROOT_DIR := ../..
PLANTRI_DIR := $(ROOT_DIR)/vendor/plantri
INCLUDE_DIR := $(ROOT_DIR)/include
BOOST_DIR := $(ROOT_DIR)/vendor

CC ?= gcc
CXX ?= g++
AR ?= ar
RM ?= rm -f

INC_DIRS  := -I$(PLANTRI_DIR) -I$(INCLUDE_DIR) -I$(BOOST_DIR)


CFLAGS ?= -O2 -std=c11   -Wall -Wextra -Wpedantic $(INC_DIRS) \
        	-Wno-unused-parameter -Wno-sign-compare -Wno-maybe-uninitialized \
            -Wno-implicit-fallthrough -Wno-missing-field-initializers

CXXFLAGS ?= -O2 -std=c++20 -Wall -Wextra -Wpedantic $(INC_DIRS)


PLANTRI_C := $(PLANTRI_DIR)/plantri.c
SHIM_SRC := plantri_shim.c
API_SRC := plantri_api.cpp

SHIM_ABS_PATH := $(abspath $(SHIM_SRC))

BUILD_DIR  := build
TARGET_LIB := libplantri_wrapper.a

OBJS := $(BUILD_DIR)/plantri_combined.o $(BUILD_DIR)/plantri_api.o

.PHONY: all clean

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(AR) rcs $@ $^

#kompilace plantri + shim
$(BUILD_DIR)/plantri_combined.o: $(PLANTRI_C) $(SHIM_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) \
	  -Dmain=plantri_run \
	  -DPLUGIN=\"$(SHIM_ABS_PATH)\" \
	  -c $(PLANTRI_C) -o $@


$(BUILD_DIR)/plantri_api.o: $(API_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET_LIB)