#ifndef MAMAN14_UTILITIES_H
#define MAMAN14_UTILITIES_H

int symbolExist(char *);

void readGuidance(char *, int);

void readLine_firstRun(char *);

void readString(char *);

void readData(char *);

int readNextNum(char **);

void resetLine(char *);

int charIsDigit(char);

int charIsAlpha(char);

void readExtern(char *);

void addToSymbolList(char *, int, int, int);

void addToCommandList(int, int, int, char *, char *, int, int, char *);

void addToDataList(int, int, int);

int getGuidanceCode(char *);

int getOperationCode(char *);

int isLegalLabel(char *);

int wordIsSave(char *);

void readOperation(char *, int, char *);

void readNoneArgsOperation(int, char *);

void readOneArgOperation(char *, int, char *);

void readTwoArgsOperation(char *, int, char *);

char *getNextWord(char *);

char *getNextArg(char *);

int isRegister(char *);

int isRegisterAddress(char *);

int isNumber(char *);

int isDirectNumber(char *);

int getAddressReallocationMethod(char *);

void initLists();

void initCounters();

char *getGuidanceLine(char *);

int shouldSkipLine(char *);

void readline_secondRun(char *line);

int isEOF(char *word);

int isEndOfLine(char *word);

int isCommentSentence(char *word);

int isEntryGuidance(char *word);

int checkErrorList();

void addEntryToSymbols(char *symbolName, int lineNum);

int shouldSkipLineSecondRun(char *word);

void updateSymbolList();

void fixLabelName();

void fixOperationLabel();

void updateDataAddress();

int isLastWord(char *);

#endif

