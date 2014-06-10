BIN=./../
INC=./../include
INC_VEC=./../../../math/vector/include
LIB=-L /root/libftdi1-1.1.build/src #-L /usr/local/lib/
LINK=-lindicurses -lm -lcurses  -lpthread -lardutalk #-lusb-1.0

all:
	#gcc -I $(INC) testDecodeQuaternion.c $(BIN)/smash-imu-decoder -o testDecodeQuaternion	
	gcc -I $(INC) -I $(INC_VEC) $(LIB) testConnect.c -o testConnect $(BIN)/libsmash-imu.a $(LINK)	
