#include <stdio.h>

// Part 2 of the hw1

int main () {
	int x = -1;
	printf("part a is %d\n", !!(x+1));
	printf("part b is %d\n", !!x);
	printf("part c is %d\n", (!!(x | (~0U>>4)+1)));
	printf("part d is %d\n", (!!(x<<((sizeof(x)*8-4)))));
	printf("part e is %d\n", ((x| (~0U>>8))^(x<<sizeof(x)*8-8 | (~0U>>8))) == 0);
	printf("part f is %d\n", !!(x&2863311530));
	return 0;
}
