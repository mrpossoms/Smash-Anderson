INC=./include
INC_VEC=./../../math/vector/include
SRC=./src

all:
	gcc -I $(INC) -c $(SRC)/smash-imu-decoder.c -o smash-imu-decoder
	gcc -I $(INC) -I $(INC_VEC) -c $(SRC)/smash-imu.c -o smash-imu 
	ar rcs libsmash-imu.a smash-imu smash-imu-decoder
install:
	cp ./include/*.h /usr/include
	cp ./*.a /usr/lib 
