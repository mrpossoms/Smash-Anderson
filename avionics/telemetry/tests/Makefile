FLAGS=-Wint-to-pointer-cast
SRC=./src/*.c
DEP_SRC=./../src/*.c
INC=-I./include -I./../include -I./../../../hub/include
LIB=./../lib/libSmashTelemetry.a
DST=./lib
LINK= -lm -lardutalk

all:
	find ./ -iname "*.c" -exec gcc $(INC) $(DEP_SRC) {} $(LIB) $(LINK) -o {}.bin \;
install:
	echo -n "Installing the library"
	cp $(DST)/include/*.h /usr/include
	cp $(DST)/*.a /usr/lib
