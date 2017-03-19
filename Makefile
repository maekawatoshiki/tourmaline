CXX := clang++-3.8
LLVM_CONFIG := llvm-config-3.8 
CXXFLAGS := -O3 -std=c++11 -MMD -MP $(shell $(LLVM_CONFIG) --cxxflags)
LIBS := -lm $(shell $(LLVM_CONFIG) --system-libs --ldflags --libs all)

PROG := tourmaline
SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(SRCS:%.cpp=%.d)

.PHONY: clean

$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ -rdynamic $(OBJS) $(LIBS)

test: $(PROG)
	@for t in $$(ls example); do \
		echo "Compiling $$t"; \
		./tourmaline example/$$t > /dev/null || (echo "-> error!" && exit 1); \
  done
	@echo "done"

clean:
	-$(RM) $(PROG) $(OBJS) $(DEPS)

-include $(DEPS)
