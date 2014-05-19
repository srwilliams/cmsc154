// hw 1
// part 1

#include <stdio.h>

int main() {
	int endian, foo;
	foo = 1;
	endian = (int) (((char*) (&foo))[1]);
	printf("hello world, from a %s-endian machine\n", endian ? "big" : "little");
	return endian;
}
