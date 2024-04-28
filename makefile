CC=gcc
CFLAGS=-Wall -g

# 'all' target to build all executables
all: display test before after

# Target 'a' to build 'display', 'before', 'after'
a: display before after

# Target 'b' to run the executables in specified order
b: a
	./before
	./display 3
	./after
	./display 3

# Building 'display' executable
display: display.c device.c
	$(CC) $(CFLAGS) -o display display.c device.c

# Building 'test' executable
test: main.c test.c CuTest.c fileSystem.c device.c file.c
	$(CC) $(CFLAGS) -o test main.c test.c CuTest.c fileSystem.c device.c file.c -lm

# Building 'before' executable
before: beforeTest.c fileSystem.c device.c file.c
	$(CC) $(CFLAGS) -o before beforeTest.c fileSystem.c device.c file.c -lm

# Building 'after' executable
after: afterTest.c fileSystem.c device.c file.c
	$(CC) $(CFLAGS) -o after afterTest.c fileSystem.c device.c file.c -lm

# Clean up binaries
clean:
	rm -f display test before after device_file

# Custom Tests
volumeName: volumeNameTest.c fileSystem.c device.c file.c
	$(CC) $(CFLAGS) -o volumeName volumeNameTest.c fileSystem.c device.c file.c -lm

createTest: createTest.c fileSystem.c device.c file.c
	$(CC) $(CFLAGS) -o createTest createTest.c fileSystem.c device.c file.c -lm
