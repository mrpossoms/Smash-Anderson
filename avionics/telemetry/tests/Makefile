FLAGS=-Wint-to-pointer-cast
SRC=./src/*.c
INC=./../include
LIB=./../lib/libSmashTelemetry.a
LINK= -lm

all:
	find ./ -iname "*.c" -exec gcc {} $(LIB) -I$(INC) $(LINK) -o {}.bin \;
