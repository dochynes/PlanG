
CC      ?= gcc
CXX     ?= g++
AR      ?= ar
RM      ?= rm -f
MKDIR   ?= mkdir -p

BUILD_DIR := build
NAUTY_DIR := vendor/nauty/nauty2_9_1
BOOST_DIR := vendor
INC_DIRS  := -I$(NAUTY_DIR) -I$(BOOST_DIR) -Iinclude




CFLAGS   ?= -O2 -std=c11   -Wall -Wextra -Wpedantic
CXXFLAGS ?= -O2 -std=c++20 -Wall -Wextra -Wpedantic
LDFLAGS  ?=
#CXXFLAGS += -I/home/stas/plantri/boost_1_89_0 ## TODO nakopirovat do include


GENG_C := $(NAUTY_DIR)/geng.c
RNG_SRC := $(NAUTY_DIR)/naurng.c


NAUTY_CORE_SRCS := \
  $(NAUTY_DIR)/nauty.c \
  $(NAUTY_DIR)/nautil.c \
  $(NAUTY_DIR)/naugraph.c \
  $(NAUTY_DIR)/schreier.c \
  $(NAUTY_DIR)/gtools.c \
  $(RNG_SRC)


SHIM_C   := src/geng/geng_shim.c
API_CPP  := src/geng/geng_api.cpp
DEMO_CPP := src/geng_demo.cpp


LIB_GENG_BRIDGE := $(BUILD_DIR)/libgeng_bridge.a
LIB_GENG_CPP    := $(BUILD_DIR)/libgeng_cpp.a
DEMO_EXE        := $(BUILD_DIR)/geng_demo

NAUTY_CORE_OBJS := $(patsubst $(NAUTY_DIR)/%.c,$(BUILD_DIR)/%.o,$(NAUTY_CORE_SRCS))

.PHONY: all clean run

all: $(DEMO_EXE)

$(LIB_GENG_BRIDGE): $(BUILD_DIR)/geng.o $(BUILD_DIR)/shim.o $(NAUTY_CORE_OBJS)
	$(AR) rcs $@ $^

$(BUILD_DIR)/geng.o: $(GENG_C)
	$(MKDIR) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC_DIRS) \
	  -DOUTPROC=bridge_outproc \
	  -DPRUNE=bridge_prune \
	  -DPREPRUNE=bridge_preprune \
	  -Dmain=GENG_MAIN \
	  -Wno-unused-parameter \
	  -Wno-sign-compare \
	  -Wno-unused-function \
	  -c $< -o $@

#  C shim -> shim.o
$(BUILD_DIR)/shim.o: $(SHIM_C)
	$(MKDIR) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

# nauty jadro ->  .o
$(BUILD_DIR)/%.o: $(NAUTY_DIR)/%.c
	$(MKDIR) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC_DIRS)\
	 -Wno-unused-parameter \
	  -Wno-sign-compare \
	  -Wno-unused-function \
	  -Wno-implicit-function-declaration \
	  -Wno-int-conversion \
	 -c $< -o $@ 


#  C++ API glue
$(LIB_GENG_CPP): $(BUILD_DIR)/geng_api.o
	$(AR) rcs $@ $^

$(BUILD_DIR)/geng_api.o: $(API_CPP) include/geng/GengAPI.hpp
	$(MKDIR) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

#  Demo exe
$(DEMO_EXE): $(BUILD_DIR)/geng_demo.o $(LIB_GENG_CPP) $(LIB_GENG_BRIDGE)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/geng_demo.o: $(DEMO_CPP) include/geng/GengAPI.hpp
	$(MKDIR) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

run: $(DEMO_EXE)
	./$(DEMO_EXE)


clean:
	$(RM) -r $(BUILD_DIR)