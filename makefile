GPP=g++
GCC=gcc

OUTFILE="../bin/mysql.so"

COMPILE_FLAGS=-c -O3 -w -DLINUX -I./SDK/amx/

all:
	$(GCC) $(COMPILE_FLAGS) ./SDK/amx/*.c
	$(GPP) $(COMPILE_FLAGS) ./SDK/*.cpp
	$(GPP) $(COMPILE_FLAGS) ./MySQL/*.cpp
	$(GPP) -O2 -fshort-wchar -shared -o $(OUTFILE) *.o -L/usr/local/mysql/lib/mysql/ -lmysqlclient_r -lpthread
	rm -f *.o
