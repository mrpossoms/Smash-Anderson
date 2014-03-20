BIN=./../
INC=./../include
INC_VEC=./../../../math/vector/include
LIB=-L /root/libftdi1-1.1.build/src #-L /usr/local/lib/

all:
	gcc -I $(INC) testDecodeQuaternion.c $(BIN)/smash-imu-decoder -o testDecodeQuaternion	
	gcc -I $(INC) -I $(INC_VEC) $(LIB) testConnect.c -o testConnect -lftdi1 $(BIN)/libsmash-imu.a -lusb-1.0 -lpthread -lm	
