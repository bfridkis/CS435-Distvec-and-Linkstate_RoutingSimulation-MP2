# If you use threads, add -pthread here.
CPP = g++
COMPILERFLAGS = -g -Wall -Wextra -Wno-sign-compare -std=c++2a -fmax-errors=1

# Any libraries you might need linked in.
LINKLIBS = -lpthread

# The components of each program. When you create a foo.c source file, add obj/foo.o here, separated
# by a space (e.g. SOMEOBJECTS = obj/foo.o obj/bar.o obj/baz.o).
LINKSTATEOBJECTS = OBJECTS/linkstate.o OBJECTS/converge.o OBJECTS/messagePrint.o OBJECTS/processChanges.o  \
				 OBJECTS/outputFunctions.o OBJECTS/tieBreaker.o OBJECTS/vecToString.o
DISTVECOBJECTS = OBJECTS/distvec.o OBJECTS/converge.o OBJECTS/messagePrint.o OBJECTS/processChanges.o  \
				 OBJECTS/outputFunctions.o OBJECTS/tieBreaker.o OBJECTS/vecToString.o

#Header files for dependency (Can't use wildcard below because distvec.cpp does not have a corresponding header file.)
HDR = HEADERS/converge.hpp HEADERS/messagePrint.hpp HEADERS/processChanges.hpp HEADERS/outputFunctions.hpp \
	  HEADERS/tieBreaker.hpp HEADERS/vecToString.hpp HEADERS/include.hpp

# Every rule listed here as .PHONY is "phony": when you say you want that rule satisfied,
# Make knows not to bother checking whether the file exists, it just runs the recipes regardless.
# (Usually used for rules whose targets are conceptual, rather than real files, such as 'clean'.
# If you DIDNT mark clean phony, then if there is a file named 'clean' in your directory, running
# `make clean` would do nothing!!!)
.PHONY: all clean

# The first rule in the Makefile is the default (the one chosen by plain `make`).
# Since 'all' is first in this file, both `make all` and `make` do the same thing.
# (`make obj server client talker listener` would also have the same effect).
# all : obj server client talker listener
all : OBJECTS linkstate distvec
#all : OBJECTS distvec
#all : OBJECTS linkstate

# $@: name of rule's target: linkstate or distvec, for the respective rules.
# $^: the entire dependency string (after expansions); for linkstate, it's $(LINKSTATEOBJECTS), and for distvec, it's $(DISTVECOBJECTS).
#
# So, how does all of this work? This rule is saying 
#
# "I am how you make the thing called linkstate. If the thing called linkstate is required, but doesn't 
# exist / is out of date, then the way to make it is to run 
# `$(CPP) $(COMPILERFLAGS) $^ -o $@ $(LINKLIBS)`. 
# But, you can't start doing that until you are sure all of my dependencies $(LINKSTATEOBJECTS) are up to date."
#
# In this case, LINKSTATEOBJECTS might include files like obj/linkstate.o. So, if obj/linkstate.o doesn't exist or is out of date, 
# make will first look for a rule to build it. That rule is the 'obj/%.o' one, below; the % is a wildcard.
linkstate: $(LINKSTATEOBJECTS)
	$(CPP) $(COMPILERFLAGS) $^ -o $@ $(LINKLIBS)

distvec: $(DISTVECOBJECTS)
	$(CPP) $(COMPILERFLAGS) $^ -o $@ $(LINKLIBS)

# RM is a built-in variable that defaults to "rm -f".
clean:
	$(RM) OBJECTS/*.o linkstate distvec output.txt

# $<: the first dependency in the list; here, %.cpp. (Of course, we could also have used $^).
# The % sign means "match one or more characters". You specify it in the target, and when a file
# dependency is checked, if its name matches this pattern, this rule is used. You can also use the % 
# in your list of dependencies, and it will insert whatever characters were matched for the target name.

#This won't work because distvec.cpp does not have a header file. Need to use explicitly listed header variable $(HDR) [See Above]
#OBJECTS/%.o: SOURCES/%.cpp HEADERS/%.hpp
OBJECTS/%.o: SOURCES/%.cpp $(HDR)
	$(CPP) $(COMPILERFLAGS) -c -o $@ $<

#OBJECTS/distvec.o: SOURCES/distvec.cpp
#	$(CPP) $(COMPILERFLAGS) -c -o $@ $<

OBJECTS:
	mkdir -p OBJECTS
