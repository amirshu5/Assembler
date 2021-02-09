#ifndef MAMAN14_DATASTRUCTURES_H
#define MAMAN14_DATASTRUCTURES_H

#include "constants.h"

typedef struct commandsList
{
	char operationLabel[100];
    int address;
    int code;
    int required_args;
    int srcMethod;
    int destMethod;
    char srcArg [100];
    char destArg [100];
    char fileName [100];
    int lineNumber;
    struct commandsList* next;
}CommandsList;

typedef struct dataList
{
    int address;
    int code;
    char fileName [100];
    int lineNumber;
    struct dataList* next;
}DataList;

typedef struct symbolList
{
    char name [100];
    int address;
    int isGuidance; /* 0 if not 1 if yes 2 if unknown*/
    int isEntry;
    int isExtern;
    char filename [100];
    struct symbolList* next;
}SymbolList;

typedef struct errorList
{
    char fileName [100];
    int lineNum;
    char errorName [100];
    struct errorList *next;
}ErrorList;

SymbolList * getSymbol(char *symbol);
void addError(char *error, char *filename, int lineNum);

#endif

