$(info $(SHELL))

all: main.exe

main.exe: main.o memory.o b_plus_tree.o
	g++ -o $@ $<

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<