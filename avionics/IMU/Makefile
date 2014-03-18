FTDI_INC=/usr/local/include/libftdi1/
INC=./include
SRC=./src

all:
	gcc -I $(INC) -I $(FTDI_INC) -c $(SRC)/smash-imu.c -o libsmash-imu -lftdi1 
