#include <windows.h>
#include <stdio.h>
int main(){
	char buf[512];
	unsigned long len = sizeof(buf);
	GetComputerNameA(buf, &len);
	printf("%s\n", buf);
	return 0;
}
