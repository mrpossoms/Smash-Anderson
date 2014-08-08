CC=gcc
INC=./include
INC_VEC=./../../math/vector/include
SRC=./src

all:
	$(CC) -I $(INC) -c $(SRC)/smash-imu-decoder.c -o smash-imu-decoder
	$(CC) -I $(INC) -I $(INC_VEC) -c $(SRC)/smash-imu.c -o smash-imu 
	ar rcs libsmash-imu.a smash-imu smash-imu-decoder
	rm smash-imu smash-imu-decoder 
