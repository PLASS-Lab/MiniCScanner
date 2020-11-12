// MiniCScanner.c : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Scanner.h"

FILE* sourceFile;						// miniC source program
FILE* outputFile;						// output file

extern char* tokenName[];

void icgError(int n);
void startScanner(char* fn);

int main(int argc, char *argv[])
{
	printf(" *** start of Mini C Compiler\n");
	if (argc < 2) {
		icgError(1);
		exit(1);
	}
	for (int i = 1;i < argc;i++)
	{
		startScanner(argv[i]);

	}
	return 0;
}

void icgError(int n)
{
	printf("icg_error: %d\n", n);
	//3:printf("A Mini C Source file must be specified.!!!\n");
	//"error in DCL_SPEC"
	//"error in DCL_ITEM"
}

void startScanner(char* fn)
{
	char fileName[30] = "";
	char buffer[30] = "";
	struct tokenType token;
	int errorNo = 0;
	char* fileNamePtr = NULL, * context = NULL;

	strncpy_s(fileName, sizeof(fileName), fn, sizeof(fileName) - 1);
	printf("fileName : %s\n", fileName);
	printf("   * source file name: %30s\n", fileName);

	if (fopen_s(&sourceFile, fileName, "r") != 0) {
		icgError(2);
		exit(1);
	}

	strncpy_s(buffer, sizeof(buffer), fileName, sizeof(fileName) - 1);
	fileNamePtr = strtok_s(buffer, ".", &context);
	strcat_s(buffer, sizeof(buffer), ".out");
	errorNo = fopen_s(&outputFile, buffer, "w");
	if (errorNo != 0) {

		fclose(sourceFile);

		icgError(2);
		exit(1);
	}

	// Test code for scanner
	printf(" === start of Scanner\n");

	do {
		token = scanner(sourceFile);
		// write on console

		if (token.number == tident) {
			printf("Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, token.value.id);
		}
		else if (token.number == tnumber) {
			printf("Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.num);
		}
		else if (token.number == trealnumber) {
			printf("Token %10s ( %3d, %12.6f )\n", tokenName[token.number], token.number, token.value.real_num);
		}
		else if (token.number == tcharacter) {
			printf("Token %10s ( %3d, %c )\n", tokenName[token.number], token.number, token.value.character);
		}
		else if (token.number == tstring) {
			printf("Token %10s ( %3d, %s )\n", tokenName[token.number], token.number, token.value.string);
		}
		else {
			printf("Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, "");
		}


		writeToken(token, outputFile);

	} while (token.number != teof);

	printf(" *** end   of Mini C Compiler\n");

	fclose(sourceFile);
	fclose(outputFile);
}
