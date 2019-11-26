
#CC=g++-v4.1 -O3 -static
CC=g++ -O3 -static -Wno-write-strings -D_GCC_ 


SOURCES=$(shell ls *.cpp)
OBJECTS=$(shell for file in $(SOURCES);\
                do echo -n $$file | sed -e "s/\(.*\)\.cpp/\1\.o/"; echo -n " ";\
                done)

PRGNAME=arlecore352_64bit

all:  bin

bin : objects
	echo $(OBJECTS)
	$(CC) *.o -o $(PRGNAME)
	
objects : $(OBJECTS)

%.o : %.cpp
#	removeLF.sh
	$(CC) -c $< -o $@

clean:
	rm -f *.o $(PRGNAME)

depend:
#	removeLF.sh
	$(CC) -M *.cpp > $@

include depend
