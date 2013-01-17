#include <windows.h>
#include <stdio.h>
int main(){
	char buf[512];
	unsigned long len = sizeof(buf);
	for(int i=0; i<3;i++) {
		GetComputerNameA(buf, &len);
		printf("%s\n", buf);
		Sleep(1000);
	}
	return 0;
}