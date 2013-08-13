SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
SOURCE = $(wildcard *.c)
OBJECT = $(SOURCE:.c=.o)
DEPEND = $(SOURCE:.c=.d)
LDFLAGS = $(shell pkg-config --libs lua5.1) -llua5.1 -lpng -lpthread
CPPFLAGS = $(shell pkg-config --cflags lua5.1)
CXXFLAGS = $(CPPFLAGS) -W -Wall -g -DDEBUG -DKDTREE
CXX = g++
MAIN = rt

all: $(MAIN)

depend: $(DEPENDS) $(DEPEND)

clean:
	rm -f *.o *.d $(MAIN)

$(MAIN): $(OBJECTS) $(OBJECT)
	@echo Creating $@...
	@$(CXX) -o $@ $(OBJECTS) $(OBJECT) $(LDFLAGS)
#Copy the executable to the data directory since some lua scripts requires other 
#scripts in the same directory
	@mv rt ../data

%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) -o $@ -c $(CXXFLAGS) $<

%.d: %.cpp
	@echo Building $@...
	@set -e; $(CC) -M $(CPPFLAGS) $< \
                  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
                [ -s $@ ] || rm -f $@
%.o: %.c
	@echo Compiling $<...
	@$gcc -o $@ -c $(CXXFLAGS) -pedantic $<

%.d: %.c
	@echo Building $@...
	@set -e; $(CC) -M $(CPPFLAGS) $< \
                  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
                [ -s $@ ] || rm -f $@

include $(DEPENDS) $(DEPEND)