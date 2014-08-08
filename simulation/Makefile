BIN=./../avionics/IMU
INC=-I./../avionics/IMU/include -I /Library/Frameworks/SDL2.framework/Headers
LIB=-L./../avionics/IMU #-L /usr/local/lib/
SIM_SRC=momentCalculator.c stablization.c ./drawing/shapes.c ./control/pid.c
LINK=-lindicurses -lm -lcurses  -lpthread -lardutalk -lsmash-imu
LNK_SDL=/Library/Frameworks/SDL2.framework/SDL2
LNK_GL=-lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

all:
	gcc $(INC) $(SIM_SRC) $(LNK_SDL) -o sim
	gcc $(INC) $(LIB) formulaVisualizer.c -o formulaVisualizer $(BIN)/libsmash-imu.a $(LINK) $(LNK_GL)