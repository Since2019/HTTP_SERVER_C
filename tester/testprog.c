#include <curses.h>
#include <form.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <libconfig.h>

int main(void){
	
	char * catword = "cat";
	
	int length = strlen(catword);
	
	catword++;
	
	int length2 = strlen(catword);
	
	printf("%d\n", length);
	
	printf("%d\n", catword);
	
}