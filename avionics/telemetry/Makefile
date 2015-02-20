FLAGS=-Wint-to-pointer-cast
SRC=./src/*.c
INC=-I/usr/include -I./include -I./../../hub/include
LIBS=
DST=./lib
LINK=-string /usr/lib/libardutalk.a
LIB=libSmashTelemetry.a

TSTINC=./tests/include
TST=./tests
TSTS=./tests/*.c

all:
	gcc $(INC) $(LIBS) -c $(SRC)
	ar rcs $(DST)/$(LIB) *.o
	rm *.o

testlib: $(SRC)
	gcc $(LIBS) -I$(INC) -I$(TSTINC) -c $(SRC)
	ar rcs $(TST)/lib/$(LIB) *.o
	rm *.o

tests: testlib
	$(foreach test, $(TSTS), echo $(test);)
install:
	echo 'This will install the library'
	cp ./include/*.h /usr/include
	cp $(DST)/*.a /usr/lib
clean:
	find ./ -iname "*.a" -exec rm {} \;
	find ./ -iname "*.o" -exec rm {} \;
	find ./ -iname "*.out" -exec rm {} \;
setup:
	echo 'Setting up project'
	mkdir $(TST) $(DST) $(INC) ./src
	mkdir $(TSTINC)
