CXX := clang++
CXXFLAGS := -O3 -std=c++11 -MMD -MP
LIBS := -lm

PROG := tourmaline
SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(SRCS:%.cpp=%.d)

.PHONY: clean

$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ -rdynamic $(OBJS) $(LIBS)

clean:
	-$(RM) $(PROG) $(OBJS) $(DEPS)

-include $(DEPS)
