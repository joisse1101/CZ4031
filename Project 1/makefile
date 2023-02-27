all: main.exe

main.exe: main.o memory.o
	g++ -o $@ $<

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<