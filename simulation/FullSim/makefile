all:
	g++ -I/usr/local/include/ode ./control/pid.c test.c -o sim.bin -lode -ldrawstuff -framework OpenGL -framework GLUT
clean:
	rm *.bin 