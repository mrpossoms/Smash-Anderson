BIN=./../avionics/IMU
INC=-I./../avionics/IMU/include -I /Library/Frameworks/SDL2.framework/Headers
INC_VEC=./../../../math/vector/include
LIB=-L /root/libftdi1-1.1.build/src #-L /usr/local/lib/
SIM_SRC=momentCalculator.c stablization.c ./drawing/shapes.c ./control/pid.c
LINK=-lindicurses -lm -lcurses  -lpthread -lardutalk #-lusb-1.0
LNK_SDL=/Library/Frameworks/SDL2.framework/SDL2
LNK_GL=-lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

all:
	gcc $(INC) $(SIM_SRC) $(LNK_SDL) -o sim
	gcc $(INC) -I $(INC_VEC) $(LIB) formulaVisualizer.c -o formulaVisualizer $(BIN)/libsmash-imu.a $(LINK) $(LNK_GL)