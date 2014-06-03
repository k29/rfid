all: serial.o serial.cpp serial.h tag.o tah.h tag.cpp
	g++ serial.o tag.o posix_tag_IO.cpp -o posix_tag_IO

serial.o: serial.cpp serial.h
	g++ -c serial.cpp

tag.o: tag.cpp tag.h
	g++ -c tag.cpp

clean:
	rm serial.o
	rm tag.o
	rm posix_tag_IO


arm: serialarm.o serial.cpp serial.h tagarm.o tag.h tag.cpp
	arm-linux-gnueabi-g++ serialarm.o tagarm.o posix_tag_IO.cpp -o arm_posix_tag_IO

serialarm.o: serial.cpp serial.h
	arm-linux-gnueabi-g++ -c serial.cpp -o serialarm.o

tagarm.o: tag.cpp tag.h
	arm-linux-gnueabi-g++ -c tag.cpp -o tagarm.o
