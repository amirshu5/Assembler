#include "general.h"

SymbolList *getSymbol(char *symbol)
{
	SymbolList *current;
	current = symbolHead;
	while ((current != NULL) && (strcmp(current->name, symbol) != 0))
	{
		current = current->next;
	}
	return current;
}

void addError(char *error, char *filename, int lineNum)
{
	ErrorList *tempError;
	tempError = (ErrorList *) malloc(sizeof(ErrorList));
	tempError->lineNum = lineNum;
	strcpy(tempError->errorName, error);
	strcpy(tempError->fileName, filename);
	tempError->next = NULL;
	if (errorHead == NULL)
	{
		errorHead = tempError;
		errorTail = errorHead;
	}
	else
	{
		errorTail->next = tempError;
	}
}
