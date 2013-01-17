#include <windows.h>
#include <stdio.h>
int main(){
	char buf[512];
	unsigned long len = sizeof(buf);
	GetComputerNameA(buf, &len);
	printf("GetComputerNameA: \"%s\"\n", buf);
	return 0;
}
