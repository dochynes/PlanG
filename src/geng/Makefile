ROOT_DIR := ../..
NAUTY_DIR := $(ROOT_DIR)/vendor/nauty/nauty2_9_1
INCLUDE_DIR := $(ROOT_DIR)/include
BOOST_DIR := $(ROOT_DIR)/vendor

NAUTY_CONFIG := $(NAUTY_DIR)/nauty.h
NAUTY_GENERATED_HEADERS := $(NAUTY_DIR)/nauty.h $(NAUTY_DIR)/naututil.h $(NAUTY_DIR)/gtools.h

INC_DIRS := -I$(NAUTY_DIR) -I$(INCLUDE_DIR) -I$(BOOST_DIR)

CC ?= gcc
CXX ?= g++

RM ?= rm -f

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
AR = ar
OPT_FLAGS ?= -O3 -DNDEBUG -DMAXN=WORDSIZE
else
AR = gcc-ar
OPT_FLAGS ?= -O3 -flto -march=native -DNDEBUG -DMAXN=WORDSIZE
endif

CFLAGS ?= $(OPT_FLAGS) -std=c11 -Wall -Wextra -Wpedantic $(INC_DIRS)
CXXFLAGS ?= $(OPT_FLAGS) -std=c++20 -Wall -Wextra -Wpedantic $(INC_DIRS)

NAUTY_CORE_SRCS := \
  $(NAUTY_DIR)/nauty.c \
  $(NAUTY_DIR)/nautil.c \
  $(NAUTY_DIR)/naugraph.c \
  $(NAUTY_DIR)/schreier.c \
  $(NAUTY_DIR)/gtools.c \
  $(NAUTY_DIR)/naurng.c

SHIM_SRC := geng_shim.c
API_SRC := geng_api.cpp
GENG_SRC := $(NAUTY_DIR)/geng.c


BUILD_DIR := build
OBJS := $(patsubst $(NAUTY_DIR)/%.c, $(BUILD_DIR)/nauty/%.o, $(NAUTY_CORE_SRCS))
OBJS += $(BUILD_DIR)/geng.o

OBJS += $(BUILD_DIR)/api.o

TARGET_LIB := libgeng_wrapper.a

.PHONY: all clean

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(AR) rcs $@ $^

#geng compilation
$(BUILD_DIR)/geng.o: $(GENG_SRC) $(SHIM_SRC) $(NAUTY_CONFIG)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) \
	  -DOUTPROC=bridge_outproc \
	  -DOUTPROC_ACTIVE=bridge_outproc_active \
	  -DPRUNE=bridge_filter \
	  -DPREPRUNE=bridge_preprune \
	  -DGENG_MAIN=geng_main \
	  -include $(SHIM_SRC) \
	  -Wno-unused-parameter \
	  -Wno-sign-compare \
	  -Wno-unused-function \
	  -c $< -o $@

#  c++ api kompilation
$(BUILD_DIR)/api.o: $(API_SRC) $(NAUTY_CONFIG)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# nauty core compilation
$(BUILD_DIR)/nauty/%.o: $(NAUTY_DIR)/%.c $(NAUTY_CONFIG)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) \
	 -Wno-unused-parameter \
	 -Wno-sign-compare \
	 -Wno-unused-function \
	 -Wno-implicit-function-declaration \
	 -Wno-int-conversion \
	 -c $< -o $@

$(NAUTY_GENERATED_HEADERS):
	cd $(NAUTY_DIR) && if [ -f ./configure ]; then $(SHELL) ./configure; else $(SHELL) ./configure.txt; fi

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET_LIB)
