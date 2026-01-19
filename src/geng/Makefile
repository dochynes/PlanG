ROOT_DIR := ../..
NAUTY_DIR := $(ROOT_DIR)/vendor/nauty/nauty2_9_1
INCLUDE_DIR := $(ROOT_DIR)/include
BOOST_DIR := $(ROOT_DIR)/vendor

INC_DIRS  := -I$(NAUTY_DIR) -I$(INCLUDE_DIR) -I$(BOOST_DIR)

CC ?= gcc
CXX ?= g++
AR ?= ar
RM ?= rm -f

CFLAGS   ?= -O2 -std=c11   -Wall -Wextra -Wpedantic $(INC_DIRS)
CXXFLAGS ?= -O2 -std=c++20 -Wall -Wextra -Wpedantic $(INC_DIRS)

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


#object files
BUILD_DIR := build
OBJS := $(patsubst $(NAUTY_DIR)/%.c, $(BUILD_DIR)/nauty/%.o, $(NAUTY_CORE_SRCS))
OBJS += $(BUILD_DIR)/geng.o
OBJS += $(BUILD_DIR)/shim.o
OBJS += $(BUILD_DIR)/api.o


TARGET_LIB := libgeng_wrapper.a

.PHONY: all clean

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(AR) rcs $@ $^


# geng compilation
$(BUILD_DIR)/geng.o: $(GENG_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_DIRS) \
	  -DOUTPROC=bridge_outproc \
	  -DPRUNE=bridge_prune \
	  -DPREPRUNE=bridge_preprune \
	  -Dmain=GENG_MAIN \
	  -Wno-unused-parameter \
	  -Wno-sign-compare \
	  -Wno-unused-function \
	  -c $< -o $@

# shim compilation
$(BUILD_DIR)/shim.o: $(SHIM_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

# 3. Kompilace API (C++)
$(BUILD_DIR)/api.o: $(API_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 4. Kompilace Nauty Core (Generic pattern)
$(BUILD_DIR)/nauty/%.o: $(NAUTY_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_DIRS)\
	 -Wno-unused-parameter \
	 -Wno-sign-compare \
	 -Wno-unused-function \
	 -Wno-implicit-function-declaration \
	 -Wno-int-conversion \
	 -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET_LIB)