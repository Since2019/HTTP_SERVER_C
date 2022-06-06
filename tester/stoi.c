#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getSize(int number){
	int count = 0;
	int curNum = number;
	if(curNum == 0){
		return 1;
	}
	while(curNum > 0 || curNum != 0){
		count++;
		curNum = curNum/10;
	}
	return count;
}

char* getString(int number){
	int num = number;
	char* newString = calloc(getSize(number), sizeof(char*));
	sprintf(newString, "%d", num);
	return newString;
}


int main(){
	char* string = getString(900);
	printf("%s\n", string);
	free(string);
}