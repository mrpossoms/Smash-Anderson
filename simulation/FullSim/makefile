IMU=./../../avionics/IMU
INC=-I./../../avionics/IMU/include -I/usr/local/include/ode
LIB=-L./../../avionics/IMU
LINK= -lode -ldrawstuff -lArduTalk -framework OpenGL -framework GLUT

all:
	g++ $(INC) $(LIB) ./control/pid.c test.c -o sim.bin $(IMU)/libsmash-imu.a $(LINK)
clean:
	rm *.bin 
