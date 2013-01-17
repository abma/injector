/*
    Injector (C) Matthias Ableitner 2013

    This file is part of injector.

    Injector is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation, either version 2 of the License, or (at your option)
	any later version.

    Injector is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
	or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
	for more details.

    You should have received a copy of the GNU General Public License along
	with Injector. If not, see http://www.gnu.org/licenses/
*/

#include <windows.h>
#include <stdio.h>

//program to test GetComputerNameA hook
int main(){
	char buf[512];
	unsigned long len = sizeof(buf);
	if (GetComputerNameA(buf, &len)) {
		printf("GetComputerNameA returns: \"%s\"\n", buf);
	} else {
		printf("Call of GetComputerNameA failed!\n");
	}
	return 0;
}
