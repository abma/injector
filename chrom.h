/*
	Chrom (C) Raja Jamwal 2011, <www.experiblog.co.cc> <linux1@zoho.com>

    Distributed under GNU LGPL License

    Chrom, is API/Funtion interception/hijacking library for windows systems
    
    Copyright (C) 2011  Raja Jamwal

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>

*/

#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <fstream>

using namespace std;
#define SIZE 6

struct Hook{

	FARPROC original_function;
	FARPROC destination_function;

	BYTE original_bytes[SIZE];
	BYTE JMP_instruction[SIZE];
	DWORD original_protection, new_protection;
	
	Hook(): original_function(0), destination_function(0){
		original_protection= PAGE_EXECUTE_READWRITE; 
		new_protection = PAGE_EXECUTE_READWRITE;
	}

	~Hook(){
		if (original_function!=0) {
			memcpy((void*) original_function, original_bytes, SIZE);
		} else {
			printf("couldn't restore original function!");
		}
	}

	int Initialize(const char * function, const char * module_name, const FARPROC destination_function_ptr)
	{
	
		HMODULE module_handle = GetModuleHandle(module_name);
		if (module_handle == NULL) {
			printf("GetModuleHandle failed!\n");
			return FALSE;
		}
		
		original_function = GetProcAddress(module_handle, function);
		if (original_function==NULL){
			printf("GetProcAddress(%s, %s) failed", module_name, function);
			return FALSE;
		}
		destination_function = destination_function_ptr;
		if (original_function == 0){
			return FALSE;
		}
		return TRUE;
	}

	int Start()
	{
		BYTE JMP_temporary[SIZE] = {0xE9, 0x90, 0x90, 0x90, 0x90, 0xC3};
		memcpy(JMP_instruction, JMP_temporary, SIZE);
		DWORD JMP_size = ((DWORD)destination_function - (DWORD)original_function - 5);
		VirtualProtect((LPVOID)original_function, SIZE, PAGE_EXECUTE_READWRITE, &original_protection);
		memcpy(original_bytes,(void*)original_function, SIZE);
		memcpy(&JMP_instruction[1], &JMP_size, 4);
		memcpy((void*)original_function, JMP_instruction, SIZE);
		VirtualProtect((LPVOID)original_function, SIZE, original_protection, NULL);
		return TRUE;
	}

	int Reset(){
		VirtualProtect((LPVOID)original_function, SIZE, new_protection, NULL);
		memcpy((void*)original_function, original_bytes, SIZE);
		return TRUE;
	}

	int Place_Hook(){
		memcpy((void*)original_function, JMP_instruction, SIZE);
		VirtualProtect((LPVOID)original_function, SIZE, original_protection, NULL);
		return TRUE;
	}

};
