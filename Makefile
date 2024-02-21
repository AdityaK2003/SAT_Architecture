# Usage: "make experiments; ./experiments"

CXX := g++ -g
CXXFLAGS := -std=c++1y

SRCS := old_funcs.cpp funcs.cpp experiments.cpp
OBJS := $(SRCS:.cpp=.o)

TARGET := experiments

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f tmp*.txt

