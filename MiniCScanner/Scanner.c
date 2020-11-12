/***************************************************************
 *      Scanner routine for Mini C language                    *
 *                                   2020. 5. 11               *
 ***************************************************************/

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "Scanner.h"

/*
    - 인식 키워드 추가: char, double, string,for, switch, case, default, continue, break 
    - 인식 리터럴 추가: 문자, 실수, 문자열 
    - 자바기준 documented comment /** ~ *\/ 
 */ 

enum tsymbol tnum[NO_KEYWORD] = {
    tconst,    telse,     tif,     tint,     treturn,   tvoid,     twhile,
    tchar,  tdouble, tfor, tswitch, tcase,   tdefault, tcontinue, tbreak
};

char* tokenName[] = {
    "!",        "!=",      "%",       "%=",     "%ident",   "%number",
    /* 0          1           2         3          4          5        */
    "&&",       "(",       ")",       "*",      "*=",       "+",
    /* 6          7           8         9         10         11        */
    "++",       "+=",      ",",       "-",      "--",	    "-=",
    /* 12         13         14        15         16         17        */
    "/",        "/=",      ";",       "<",      "<=",       "=",
    /* 18         19         20        21         22         23        */
    "==",       ">",       ">=",      "[",      "]",        "eof",
    /* 24         25         26        27         28         29        */
    "%realnumber", "%character", "%string",
    /* 30             31             32  실수, 문자, 문자열 literal 추가 */
    //   ...........    word symbols ................................. //
    "const",    "else",     "if",      "int",     "return",  "void",
    /* 33         34         35        36         37         38        */
    "while",    "{",        "||",       "}",      "char",    "double",
    /* 39         40         41        42         43         44        */
    "for",      "switch",   "case",   "default",  "continue", "break",
    /* 45       46          47        48          49          50       */
};

char* keyword[NO_KEYWORD] = {
    /* 정수, 실수, 문자열 등의 keyword 추가 */
    "const", "else",   "if",  "int",    "return", "void",    "while",
    "char",  "double", "for", "switch", "case",   "default", "continue", "break"
};

struct tokenType scanner(FILE *sourceFile)
{
    struct tokenType token;
    int i, index;
    char ch, id[ID_LENGTH];

    token.number = tnull;

    do {
        while (isspace(ch = fgetc(sourceFile)));	// state 1: skip blanks
        printf("%c", ch);
        if (superLetter(ch)) { // identifier or keyword
            i = 0;
            do {
                if (i < ID_LENGTH) id[i++] = ch;
                ch = fgetc(sourceFile);
            } while (superLetterOrDigit(ch));
            // identifier의 길이는 12를 넘을 수 없다
            if (i >= ID_LENGTH) lexicalError(1);
            id[i] = '\0';
            printf("id : %s\n", id);
            ungetc(ch, sourceFile);  //  retract

            // find the identifier in the keyword table
            for (index = 0; index < NO_KEYWORD; index++)
                if (!strcmp(id, keyword[index])) break;

            if (index < NO_KEYWORD)    // found, keyword exit
                token.number = tnum[index];
            else {                     // not found, identifier exit
                token.number = tident;
                strcpy_s(token.value.id, ID_LENGTH, id);
            }
        }  // end of identifier or keyword
        else if (isdigit(ch)) {  // number
            /*
                floating point가 있으면 실수, 없으면 정수.
                첫 문자에 판단 및 두 계산을 동시에 진행하는 것은 불가능하므로
                정수 부분을 계산한 후, 남은 문자에 floating point가 존재하면 실수인 것으로 판단하고 추가 계산.
            */
            char tmp_ch = ch;
            // 임시로 정수값을 저장
            int tmp_num = getNumber(sourceFile, ch);
            ch = fgetc(sourceFile);
            if (ch == '.') {
                // floating point가 있는 경우 실수로 판단하고 나머지 부분도 계산해야함.
                float tmp_float = getDecPoint(sourceFile);
                int tmp_exp = 1;
                ch = fgetc(sourceFile);
                if (ch == 'e') {
                    // 지수부가 존재하면 계산.
                    tmp_exp = getExpNum(sourceFile, ch);
                }
                token.number = trealnumber;
                token.value.real_num = ((float)(tmp_num) + tmp_float)*pow(10, tmp_exp);
            }
            else {
                // 실수가 아닌 경우 정수로 판단.
                token.number = tnumber;
                token.value.num = tmp_num;
                ungetc(ch, sourceFile);  /*  retract  */
            }
        }
        else if (ch == '\'') // character
        {
            /*
              문자는 작은 따옴표 ' 다음 문자 그리고 ' 가 순서대로 나열된다.
            */
            token.number = tcharacter;
            token.value.character = fgetc(sourceFile); // 문자는 하나만 받을 수 있음.
            char endc = fgetc(sourceFile);
            //항상 ' 으로 끝나야 함.
            if (endc != '\'')
            {
                lexicalError(5);
            }
        }
        else if (ch == '"') // string
        {   // 
            token.number = tstring;
            strcpy_s(token.value.string, sizeof(token.value.string), getString(sourceFile));
        }
        else switch (ch) {  // special character
            case '/':
                ch = fgetc(sourceFile);
                if (ch == '*')			// text comment
                    do {
                        while (ch != '*') ch = fgetc(sourceFile);
                        ch = fgetc(sourceFile);
                    } while (ch != '/');
                else if (ch == '/')		// line comment
                    while (fgetc(sourceFile) != '\n');
                else if (ch == '=')  token.number = tdivAssign;
                else {
                    token.number = tdiv;
                    ungetc(ch, sourceFile); // retract
                }
                break;
            case '!':
                ch = fgetc(sourceFile);
                if (ch == '=')  token.number = tnotequ;
                else {
                    token.number = tnot;
                    ungetc(ch, sourceFile); // retract
                }
                break;
            case '%':
                ch = fgetc(sourceFile);
                if (ch == '=') {
                    token.number = tremAssign;
                }
                else {
                    token.number = tremainder;
                    ungetc(ch, sourceFile);
                }
                break;
            case '&':
                ch = fgetc(sourceFile);
                if (ch == '&')  token.number = tand;
                else {
                    lexicalError(2);
                    ungetc(ch, sourceFile);  // retract
                }
                break;
            case '*':
                ch = fgetc(sourceFile);
                if (ch == '=')  token.number = tmulAssign;
                else {
                    token.number = tmul;
                    ungetc(ch, sourceFile);  // retract
                }
                break;
            case '+':
                ch = fgetc(sourceFile);
                if (ch == '+')  token.number = tinc;
                else if (ch == '=') token.number = taddAssign;
                else {
                    token.number = tplus;
                    ungetc(ch, sourceFile);  // retract
                }
                break;
            case '-':
                ch = fgetc(sourceFile);
                if (ch == '-')  token.number = tdec;
                else if (ch == '=') token.number = tsubAssign;
                else {
                    token.number = tminus;
                    ungetc(ch, sourceFile);  // retract
                }
                break;
            case '<':
                ch = fgetc(sourceFile);
                if (ch == '=') token.number = tlesse;
                else {
                    token.number = tless;
                    ungetc(ch, sourceFile);  // retract
                }
                break;
            case '=':
                ch = fgetc(sourceFile);
                if (ch == '=')  token.number = tequal;
                else {
                    token.number = tassign;
                    ungetc(ch, sourceFile);  // retract
                }
                break;
            case '>':
                ch = fgetc(sourceFile);
                if (ch == '=') token.number = tgreate;
                else {
                    token.number = tgreat;
                    ungetc(ch, sourceFile);  // retract
                }
                break;
            case '|':
                ch = fgetc(sourceFile);
                if (ch == '|')  token.number = tor;
                else {
                    lexicalError(3);
                    ungetc(ch, sourceFile);  // retract
                }
                break;
            case '(': token.number = tlparen;         break;
            case ')': token.number = trparen;         break;
            case ',': token.number = tcomma;          break;
            case ';': token.number = tsemicolon;      break;
            case '[': token.number = tlbracket;       break;
            case ']': token.number = trbracket;       break;
            case '{': token.number = tlbrace;         break;
            case '}': token.number = trbrace;         break;
            case EOF: token.number = teof;            break;
            default: {
                printf("Current character : %c", ch);
                lexicalError(4);
                break;
            }

        } // switch end
    } while (token.number == tnull);
    return token;
} // end of scanner

void lexicalError(int n)
{
    printf(" *** Lexical Error : ");
    switch (n) {
    case 1: printf("an identifier length must be less than 12.\n");
        break;
    case 2: printf("next character must be &\n");
        break;
    case 3: printf("next character must be |\n");
        break;
    case 4: printf("invalid character\n");
        break;
    case 5: printf("character form must be 'your character'. \n");
        break;
    case 6: printf("length of string must be less than 22.\n");
        break;
    default:
        break;
    }
}

int superLetter(char ch)
{
    if (isalpha(ch) || ch == '_') return 1;
    else return 0;
}

int superLetterOrDigit(char ch)
{
    if (isalnum(ch) || ch == '_') return 1;
    else return 0;
}

char* getString(FILE* sourcefile)
{
    /*
        string의 길이는 22('\0' 포함)으로 고정.
    */
    char string[22] = "";
    char end = 0;

    char ch = fgetc(sourcefile);
    do {
        if (ch == '\\') {
            /* \ 가 입력으로 들어온 경우
               \을 제외한 모든 문자열을 한번 받고 돌아온다.
            */
            string[end++] = ch;
            ch = fgetc(sourcefile);
            string[end++] = ch;
        }
        else {
            // 나머지 경우는 문자열에 붙임
            string[end++] = ch;
        }
        ch = fgetc(sourcefile);
    } while (ch != '"');
    string[end] = '\0';
    return string;
}

int getNumber(FILE *sourceFile, char firstCharacter)
{
    int num = 0;
    int value;
    char ch;

    if (firstCharacter == '0') {
        ch = fgetc(sourceFile);
        if ((ch == 'X') || (ch == 'x')) {		// hexa decimal
            while ((value = hexValue(ch = fgetc(sourceFile))) != -1)
                num = 16 * num + value;
        }
        else if ((ch >= '0') && (ch <= '7'))	// octal
            do {
                num = 8 * num + (int)(ch - '0');
                ch = fgetc(sourceFile);
            } while ((ch >= '0') && (ch <= '7'));
        else num = 0;						// zero
    }
    else {									// decimal
        ch = firstCharacter;
        do {
            num = 10 * num + (int)(ch - '0');
            ch = fgetc(sourceFile);
        } while (isdigit(ch));
    } 
    ungetc(ch, sourceFile);  /*  retract  */
    return num;
}

float getDecPoint(FILE* sourceFile)
{
    // floating point를 읽는 함수
    float num = 0.0;
    int exp=-1;
    char ch;
    // 실수 부분 읽기
    ch = fgetc(sourceFile); // '.' 다음 정수를 읽음.
    while (isdigit(ch)) {
        num += (double)((ch - '0') * pow(10, exp--));
        ch = fgetc(sourceFile);
    }
    ungetc(ch, sourceFile);  /*  retract  */
    return num;
}

int getExpNum(FILE* sourceFile)
{
    // 지수부를 계산하는 함수
    char ch;
    int exp_num = 0;    // 뒤에 숫자가 안붙는 경우는 0으로 처리
    int flag = 1;
    ch = fgetc(sourceFile);
    /*
        + or 바로 숫자가 나오는 경우 양의 지수
        - 가 나오는 경우 음의 지수로 판단.
    */
    if (ch == '+') {
        ch = fgetc(sourceFile);
    }
    else if (ch == '-') {
        flag = -1;
        ch = fgetc(sourceFile);
    }
    if (isdigit(ch))
    {
        do {
            exp_num += (10 * exp_num + (int)(ch - '0'));
            ch = fgetc(sourceFile);
        } while (isdigit(ch));
    }
    exp_num *= flag;
    ungetc(ch, sourceFile);  /*  retract  */
    return exp_num;
}

int hexValue(char ch)
{
    switch (ch) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return (ch - '0');
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return (ch - 'A' + 10);
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return (ch - 'a' + 10);
    default: return -1;
    }
}

void writeToken(struct tokenType token, FILE *outputFile)
{   // 
    if (token.number == tident) {
        fprintf(outputFile, "Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, token.value.id);
    }
    else if (token.number == tnumber) {
        fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.num);
    }
    else if (token.number == trealnumber) {
        fprintf(outputFile, "Token %10s ( %3d, %12.6f )\n", tokenName[token.number], token.number, token.value.real_num);
    }
    else if (token.number == tcharacter) {
        fprintf(outputFile, "Token %10s ( %3d, %c )\n", tokenName[token.number], token.number, token.value.character);
    }
    else if (token.number == tstring) {
        fprintf(outputFile, "Token %10s ( %3d, %s )\n", tokenName[token.number], token.number, token.value.string);
    }
    else {
        fprintf(outputFile, "Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, "");
    }
    
}