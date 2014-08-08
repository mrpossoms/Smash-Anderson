CC=gcc
BIN=./../
INC=-I./../include
INC_VEC=./../../../math/vector/include
LIB=-L /root/libftdi1-1.1.build/src #-L /usr/local/lib/
LINK=-lindicurses -lm -lcurses  -lpthread -lardutalk #-lusb-1.0
LNK_GL=-lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

all:
	#gcc -I $(INC) testDecodeQuaternion.c $(BIN)/smash-imu-decoder -o testDecodeQuaternion	
	$(CC) $(INC) -I $(INC_VEC) $(LIB) testConnect.c -o testConnect $(BIN)/libsmash-imu.a $(LINK)
	$(CC) $(INC) -I $(INC_VEC) $(LIB) testGizmo.c -o testGizmo $(BIN)/libsmash-imu.a $(LINK) $(LNK_GL)	
