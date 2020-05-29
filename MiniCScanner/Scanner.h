
/***************************************************************
 *      Header file of scanner routine for Mini C language     *
 *                                   2020. 5. 11               *
 ***************************************************************/

#ifndef SCANNER_H

#define SCANNER_H 1


#include <stdio.h>

#define NO_KEYWORD 15
#define ID_LENGTH 30

struct tokenType {
    int number;
    union {
        /*
            �Ǽ����� ����, ���ڿ� ���� �߰�.
            ID_LENGTH�� 30���� �ø�.
        */
        char id[ID_LENGTH];
        int num;        // ������
        float real_num; // �Ǽ���
        char character; // ������
        char string[22]; // ���ڿ� (string�� ��� symbol�� ���̰� 7(%string)�̱� ������ 22�� ����.)
    } value;
};

enum tsymbol {
    tnull = -1,
    tnot, tnotequ, tremainder, tremAssign, tident, tnumber,
    /* 0          1            2         3            4          5     */
    tand, tlparen, trparen, tmul, tmulAssign, tplus,
    /* 6          7            8         9           10         11     */
    tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
    /* 12         13          14        15           16         17     */
    tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,
    /* 18         19          20        21           22         23     */
    tequal, tgreat, tgreate, tlbracket, trbracket, teof,
    /* 24         25          26        27           28         29     */
    trealnumber, tcharacter, tstring,
    /* 30             31             32  �Ǽ�, ����, ���ڿ� literal �߰� */
    //   ...........    word symbols ................................. //
    /* 33         34         35        36         37         38        */
    tconst,       telse,      tif,      tint,        treturn,   tvoid,
    /* 39         40         41        42         43         44        */
    twhile,       tlbrace,    tor,      trbrace,     tchar,     tdouble,
    /* 45       46          47        48          49          50       */
    tfor,         tswitch,    tcase,    tdefault,    tcontinue, tbreak,
};


int superLetter(char ch);
int superLetterOrDigit(char ch);
char* getString(FILE* sourcefile);
int getNumber(FILE *sourceFile, char firstCharacter);
float getDecPoint(FILE* sourceFile);
int getExpNum(FILE* sourceFile);
int hexValue(char ch);
void lexicalError(int n);
struct tokenType scanner(FILE *sourceFile);
void writeToken(struct tokenType token, FILE* outputFile);
#endif // !SCANNER_H

