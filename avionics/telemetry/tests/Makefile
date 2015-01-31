FLAGS=-Wint-to-pointer-cast
SRC=./src/*.c
DEP_SRC=./../src/*.c
INC=-I/usr/include -I./include -I./../include -I./../../../hub/include
LIB=-L /usr/lib
#LIB=./../lib/libSmashTelemetry.a
DST=./lib
LINK= -lm /usr/lib/libardutalk.a

all:
	find ./ -iname "*.c" -exec gcc $(LIB) $(INC) $(DEP_SRC) {} $(LINK) -o {}.bin \;

install:
	echo -n "Installing the library"
	cp $(DST)/include/*.h /usr/include
	cp $(DST)/*.a /usr/lib
