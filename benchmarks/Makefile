CXX ?= g++
RM ?= rm -f

ROOT := ..
LIB_DIR := $(ROOT)/lib

INC_FLAGS := -I$(ROOT)/include -I$(ROOT)/vendor -I$(ROOT)/vendor/nauty/nauty2_9_1
OPT_FLAGS ?= -O3 -flto -march=native -DNDEBUG
CXXFLAGS ?= $(OPT_FLAGS) -std=c++20 -Wall
LDLIBS := -L$(LIB_DIR) -lgeng_wrapper -lplantri_wrapper

LIB_DEPS := $(LIB_DIR)/libgeng_wrapper.a $(LIB_DIR)/libplantri_wrapper.a
TARGETS := bench_geng bench_plantri dump_compare count_plantri_maxdeg

.PHONY: all clean

all: $(TARGETS)

$(LIB_DEPS):
	$(MAKE) -C $(ROOT) libs

bench_geng: bench_geng.cpp $(LIB_DEPS)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) $< $(LDLIBS) -o $@

bench_plantri: bench_plantri.cpp $(LIB_DEPS)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) $< $(LDLIBS) -o $@

dump_compare: dump_compare.cpp $(LIB_DEPS)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) $< $(LDLIBS) -o $@

count_plantri_maxdeg: count_plantri_maxdeg.cpp $(LIB_DEPS)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) $< $(LDLIBS) -o $@

clean:
	$(RM) $(TARGETS)
