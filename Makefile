CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic

SRC := lexer.cpp
OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

TARGET := lexer

all: $(TARGET) clean

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP)

fclean: clean
	rm -f $(TARGET)

.PHONY: all clean