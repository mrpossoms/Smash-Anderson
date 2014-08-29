FLAGS=-Wint-to-pointer-cast
SRC=./src/*.c
INC=-I./include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
LIB=./../lib/libSmashTelemetry.a
DST=./lib
LINK= -lm -lardutalk

all:
	find ./ -iname "*.c" -exec gcc $(INC) {} $(LIB) $(LINK) -o {}.bin \;
install:
	echo -n "Installing the library"
	cp $(DST)/include/*.h /usr/include
	cp $(DST)/*.a /usr/lib