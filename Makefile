SRCDIR := src
TESTDIR := test

OBJDIR := obj
DEPDIR := dep
BINDIR := bin

SOBJDIR := obj/src
SDEPDIR := dep/src
TOBJDIR := obj/test
TDEPDIR := dep/test

TESTEXEC := test.out
MAINEXEC := main.out

SRCS := $(shell find $(SRCDIR) -name "*.cpp")
SOBJS := $(SRCS:$(SRCDIR)/%.cpp=$(SOBJDIR)/%.o)
SDEPS := $(SRCS:$(SRCDIR)/%.cpp=$(SDEPDIR)/%.d)
STREE := $(patsubst %/,%,$(dir $(SOBJS)))

TESTS := $(shell find $(TESTDIR) -name "*.cpp")
TOBJS := $(TESTS:$(TESTDIR)/%.cpp=$(TOBJDIR)/%.o)
TDEPS := $(TESTS:$(TESTDIR)/%.cpp=$(TDEPDIR)/%.d)
TTREE := $(patsubst %/,%,$(dir $(TOBJS)))

SCPPFLAGS  = -MMD -MP -MF $(@:$(SOBJDIR)/%.o=$(SDEPDIR)/%.d)
TCPPFLAGS  = -MMD -MP -MF $(@:$(TOBJDIR)/%.o=$(TDEPDIR)/%.d)

BCXXFLAGS = -I$(SRCDIR) -I$(TESTDIR) `pkg-config --cflags libsodium` -O0 -g -std=c++2a -fcoroutines-ts -stdlib=libc++
BLINKERFLAGS = `pkg-config --libs libsodium` -stdlib=libc++ -lc++abi -pthread

all: CXXFLAGS = $(BCXXFLAGS) -D __main__
test: CXXFLAGS = $(BCXXFLAGS) -D __test__

.PHONY: all test clean

all: $(BINDIR)/$(MAINEXEC)

test: $(BINDIR)/$(TESTEXEC)
	$(BINDIR)/$(TESTEXEC)

clean:
	$(RM) -r $(OBJDIR) $(DEPDIR) $(BINDIR)

.SECONDEXPANSION:
$(SOBJDIR)/%.o: $(SRCDIR)/%.cpp | $$(@D)
	$(CXX) $(SCPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(TOBJDIR)/%.o: $(TESTDIR)/%.cpp | $$(@D)
	$(CXX) $(TCPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(BINDIR):
	mkdir -p $(BINDIR)

$(STREE): %:
	mkdir -p $@
	mkdir -p $(@:$(SOBJDIR)%=$(SDEPDIR)%)

$(TTREE): %:
	mkdir -p $@
	mkdir -p $(@:$(TOBJDIR)%=$(TDEPDIR)%)

$(BINDIR)/$(TESTEXEC): $(SOBJS) $(TOBJS) $(BINDIR)
	$(CXX) -o $(BINDIR)/$(TESTEXEC) $(SOBJS) $(TOBJS)

$(BINDIR)/$(MAINEXEC): $(SOBJS) $(BINDIR)
	$(CXX) $(BLINKERFLAGS) $(LINKERFLAGS) -o $(BINDIR)/$(MAINEXEC) $(SOBJS)
ifeq "$(MAKECMDGOALS)" ""
-include $(SDEPS)
-include $(TDEPS)
endif
