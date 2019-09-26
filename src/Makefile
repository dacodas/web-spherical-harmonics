# https://spin.atomicobject.com/2016/08/26/makefile-c-projects/

TARGET_EXEC ?= texture-generator

BUILD_DIR ?= ./build
SRC_DIRS ?= ./
CXX = g++

CXXFLAGS += -std=c++17 -fopenmp $(shell pkg-config --cflags MagickWand)
LDFLAGS ?= $(shell pkg-config --cflags --libs MagickWand) -lfcgi -lgsl -lstdc++fs

SRCS := $(shell find $(SRC_DIRS) -name \*.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
