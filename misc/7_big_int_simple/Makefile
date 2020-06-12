CXX := clang++
LD := $(GCC)

BUILD_DIR := build

SRCS := $(shell find . -name "*.cpp" -printf '%P\n')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
INCL := $(wildcard *.h)

#CPPFLAGS := $(BOTH_FLAGS) $(INC_FLAGS)
CPPFLAGS := $(BOTH_FLAGS) $(INC_FLAGS)
CPPFLAGS += -Wall -Wextra -std=c++20 -Og
#-ferror-limit=10
#-Wno-unused-const-variable \
#-DOHMD_STATIC
#



#LDFLAGS := -static-libstdc++ -static -pthread
LDFLAGS := 
#-ldbghelp

#this must be the last
#LDFLAGS += -limagehlp

.PHONY: build
build: $(TARGET)

.PHONY: rebuild
rebuild: clean build

.PHONY: compile
compile: clean $(OBJS)

$(TARGET): $(OBJS)
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

#$(BUILD_DIR)/%.cpp.o: %.cpp ${INCL}

$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $<


.PHONY: clean
clean:
	@$(RM) $(BUILD_DIR)/*.o

MKDIR_P ?= mkdir -p
