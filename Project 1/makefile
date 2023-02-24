TARGET = main.exe

OBJS = \
	memory.o \
	main.o

REBUILDABLES = $(OBJS) $(TARGET)

all : $(LINK_TARGET)
	echo All done

$(LINK_TARGET) : $(OBJS)
	g++ -g -o $@ $^

%.o : %.cpp
	g++ -g -o $@ -c $<

main.o: memory.h
memory.o: memory.h