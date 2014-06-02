all: serial.o serial.cpp serial.h
	g++ serial.o posix_read_write.cpp -o posix_read_write

serial.o: serial.cpp serial.h
	g++ -c serial.cpp

clean:
	rm serial.o
	rm posix_read_write

arm: serialarm.o serial.cpp serial.h
	arm-linux-gnueabi-g++ serialarm.o posix_read_write.cpp -o arm_posix_read_write

serialarm.o: serial.cpp serial.h
	arm-linux-gnueabi-g++ -c serial.cpp -o serialarm.o
