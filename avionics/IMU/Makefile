CC=gcc
INC=-I./include -I./../../hub/include
INC_VEC=./../../math/vector/include
SRC=./src/*.c
LINK=-lardutalk -lpthread

all:
	$(CC) $(INC) -c $(SRC)/smash-imu-decoder.c -o smash-imu-decoder
	$(CC) $(INC) -I $(INC_VEC) -c $(SRC)/smash-imu.c -o smash-imu 
	ar rcs libsmash-imu.a smash-imu smash-imu-decoder
	rm smash-imu smash-imu-decoder
worker:
	$(CC) $(INC) $(SRC) main.c -o smash-imu $(LINK)
