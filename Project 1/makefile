all: main.exe

main.exe: main.o memory.o
	g++ -o $@ $<
	rm -f $<

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<