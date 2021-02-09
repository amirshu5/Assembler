#include "general.h"

int shouldSkipLine(char *word)
{
	return (isEOF(word) || isEndOfLine(word) || isCommentSentence(word) || isEntryGuidance(word));
}

int isEOF(char *word)
{
	return (word == NULL);
}

int isEndOfLine(char *word)
{
	return (strcmp(word, "\n") == 0);
}

int isCommentSentence(char *word)
{
	return (strcmp(word, ";") == 0);
}

int isEntryGuidance(char *word)
{
	return (strcmp(word, ".entry") == 0);
}

void readLine_firstRun(char *line)
{
	char *word;
	char *labelName;
	char delim[3];
	int isExtern;
	int tempDC;
	int tempIC;
	int guidanceCode;
	int operationCode;
	delim[0] = ' ';
	delim[1] = '\t';
	delim[2] = ',';
	word = strtok(line, delim);

	if (shouldSkipLine(word))
	{
		return;
	}

	else if (isLegalLabel(word) == LEGAL_FIRST_WORD)
	{
		labelName = word;
		if (symbolExist(labelName) == IS_EXIST)
		{
			addError(DuplicateSymbolError, fileName, lineNumber);
			return;
		}
		word = getNextWord(word);
		guidanceCode = getGuidanceCode(word);
		operationCode = getOperationCode(word);
		if (guidanceCode != FALSE)
		{
			tempDC = dc;
			word = getGuidanceLine(word);
			readGuidance(word, guidanceCode);
			isExtern = (guidanceCode == IS_EXTERN);
			addToSymbolList(labelName, tempDC, 1, isExtern);
		}
		else if (operationCode != FALSE)
		{
			tempIC = ic;
			word = getNextWord(word);
			readOperation(word, operationCode, labelName);
			addToSymbolList(labelName, tempIC, 0, 0);
		}
		else
		{
			addError(UnknownCommandInLine, fileName, lineNumber);
		}
	}

	else
	{
		guidanceCode = getGuidanceCode(word);
		operationCode = getOperationCode(word);
		if (guidanceCode != FALSE)
		{
			word = getGuidanceLine(word);
			readGuidance(word, guidanceCode);
		}
		else if (operationCode != FALSE)
		{
			word = getNextWord(word);
			readOperation(word, operationCode, NO_LABEL);
		}
		else
		{
			addError(UnknownSymbol, fileName, lineNumber);
		}
	}
}

/**
 * read operation sentence
 */
void readOperation(char *word, int operationCode, char *label)
{
	if (0 <= operationCode && operationCode <= 4)/*operation with two args*/
	{
		readTwoArgsOperation(word, operationCode, label);
	}
	else if (5 <= operationCode && operationCode <= 13)/*operation with one arg*/
	{
		readOneArgOperation(word, operationCode, label);
	}
	else /*operation without args*/
	{
		readNoneArgsOperation(operationCode, label);
	}
}

/**
 * read operation sentence with 2 argument operation
 */
void readTwoArgsOperation(char *word, int operationCode, char *label)
{
	char *srcArg;
	char *destArg;
	int srcMethod;
	int destMethod;
	int lastWord;
	srcArg = word;
	destArg = getNextArg(word);
	srcMethod = getAddressReallocationMethod(srcArg);
	destMethod = getAddressReallocationMethod(destArg);
	lastWord = isLastWord(word);
	if (srcMethod != ERROR && destMethod != ERROR && lastWord == TRUE)
	{
		if (operationCode == 0 || operationCode == 2 || operationCode == 3)
		{
			if ((srcMethod == 2 || srcMethod == 3) && (destMethod == 2 || destMethod == 3))
			{
				addToCommandList(ic, operationCode, 2, srcArg, destArg, srcMethod, destMethod, label);
				ic += 2;
			}
			else if (destMethod >= 1)
			{
				addToCommandList(ic, operationCode, 2, srcArg, destArg, srcMethod, destMethod, label);
				ic += 3;
			}

		}
		else if (operationCode == 1)
		{
			if ((srcMethod == 2 || srcMethod == 3) && (destMethod == 2 || destMethod == 3))
			{
				addToCommandList(ic, operationCode, 2, srcArg, destArg, srcMethod, destMethod, label);
				ic += 2;
			}
			else
			{
				addToCommandList(ic, operationCode, 2, srcArg, destArg, srcMethod, destMethod, label);
				ic += 3;
			}
		}
		else
		{
			if (srcMethod == 1 && destMethod >= 1)
			{
				addToCommandList(ic, operationCode, 2, srcArg, destArg, srcMethod, destMethod, label);
				ic += 3;
			}
		}
	}
	else
	{
		if (lastWord == FALSE)
		{
			addError(IllegalCommandFormat, fileName, lineNumber);
		}
		else
		{
			addError(IllegalAddressingMethod, fileName, lineNumber);
		}
	}
}

/**
 * read operation sentence with 1 argument operation
 */
void readOneArgOperation(char *word, int operationCode, char *label)
{
	int destMethod;
	int lastWord;
	destMethod = getAddressReallocationMethod(word);
	lastWord = isLastWord(word);

	if (destMethod == ERROR)
	{
		addError(IllegalAddressingMethod, fileName, lineNumber);
	}
	else if (lastWord == FALSE)
	{
		addError(IllegalCommandFormat, fileName, lineNumber);
	}

	else if ((operationCode >= 5 && operationCode <= 8) || operationCode == 11)
	{
		if (destMethod != 0)
		{
			addToCommandList(ic, operationCode, 1, NOT_USED_STRING, word, NOT_USED_INT, destMethod, label);
			ic += 2;
		}
		else
		{
			addError(IllegalAddressingMethod, fileName, lineNumber);
		}
	}

	else if (operationCode == 9 || operationCode == 10 || operationCode == 13)
	{
		if (destMethod == 2 || destMethod == 1)
		{
			addToCommandList(ic, operationCode, 1, NOT_USED_STRING, word, NOT_USED_INT, destMethod, label);
			ic += 2;
		}
		else
		{
			addError(IllegalAddressingMethod, fileName, lineNumber);
		}
	}

	else if (operationCode == 12)
	{
		addToCommandList(ic, operationCode, 1, NOT_USED_STRING, word, NOT_USED_INT, destMethod, label);
		ic += 2;
	}
}

/**
 * read operation sentence without argument operation
 */
void readNoneArgsOperation(int operationCode, char *label)
{
	int lastWord;
	lastWord = isLastWord(label);
	if (lastWord == FALSE)
	{
		addError(IllegalCommandFormat, fileName, lineNumber);
	}
	else
	{
		addToCommandList(ic, operationCode, 0, NOT_USED_STRING, NOT_USED_STRING, NOT_USED_INT, NOT_USED_INT, label);
		ic++;
	}
}

/**
 * check if word is register
 * return 1 if word is register 0 if not
 */
int isRegister(char *word)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		if (strcmp(word, REGISTER[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * check if word is address of register
 * return 1 if word is address of register 0 if not
 */
int isRegisterAddress(char *word)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		if (strcmp(word, REGISTER_ADDRESS[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * check the operation and return the number of the operation
 * return 0 if mov, 1 if cmp, ..., 15 if stop , -1 if not operation
 */
int getOperationCode(char *word)
{
	int i;
	int length;
	length = strlen(word);
	if (word[length - 2] == '\r')
	{
		word[length - 2] = '\0';
	}
	else if (word[length - 1] == '\n')
	{
		word[length - 1] = '\0';
	}
	for (i = 0; i < 16; i++)
	{
		if (strcmp(word, COMMANDS[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}


/**
 * read guidance sentence
 */
void readGuidance(char *word, int guidanceCode)
{
	if (guidanceCode == DATA)
	{
		readData(word);
	}
	else if (guidanceCode == STRING)
	{
		readString(word);
	}
	else
	{
		readExtern(word);
	}
}

/**
 * read extern sentence
 */
void readExtern(char *word)
{
	char labelName[80];
	int i;
	i = 0;
	while (*word != ' ' && *word != '\t' && *word != '\r' && *word != '\n' && *word != '\0')
	{
		labelName[i] = *word;
		word = word + 1;
		i++;
	}
	labelName[i] = '\0';
	if (isLegalLabel(labelName) == 1 && symbolExist(labelName) == 0)
	{
		addToSymbolList(labelName, 0, 2, 1);
	}
	else
	{
		addError(IllegalLabelError, fileName, lineNumber);
	}
}

/**
 * read .string sentence
 */
void readString(char *word)
{
	char string[80];
	int status;
	int length;
	int i;
	status = 0;
	length = 0;
	while (status != 2)
	{
		if (*word == '\"')
		{
			status++;
			word = word + 1;
		}
		else
		{
			string[length] = *word;
			length++;
			word = word + 1;
		}
	}
	string[length] = '\0';
	for (i = 0; i < length + 1; i++)
	{
		addToDataList(dc, (int) string[i], TRUE);
		dc++;
	}
}

/**
 * read the numbers and insert them to the array;
 */

void readData(char *str)
{
	int number[LINE_LENGTH];
	int i;
	int k;
	int tempNum;
	i = 0;
	k = 0;
	while (str != NULL)
	{
		while (*str != '\0' && (strcmp(str, "\r\n") != 0) && (*str != '\n'))
		{
			tempNum = readNextNum(&str);
			if (tempNum != ERROR)
			{
				number[i] = tempNum;
				i++;
			}
			else
			{
				addError(IllegalNumberError, fileName, lineNumber);
			}
		}
		str = getNextWord(str);
	}
	while (k < i)
	{
		addToDataList(dc, number[k], FALSE);
		dc++;
		k++;
	}
}

/**
 * read the next number in the string
 * return the number or ERROR (-9999)
 */
int readNextNum(char **str)
{
	char number[80];
	int i;
	int num;
	int status; /* 0 mean unknown 1 mean positive number 2 mean negative number*/
	char *currChar;
	int flagWhiteSpace;
	int flagNum;
	int length; /*number of char we already read*/
	length = 0;
	flagNum = 0;
	flagWhiteSpace = 0;
	status = 0;
	i = 0;
	currChar = *str;
	while (*currChar != ',' && *currChar != '\n' && *currChar != '\r' && *currChar != '\0')
	{
		if (charIsDigit(*currChar))
		{
			if (flagWhiteSpace == 1 && status != 0)
			{
				addError(MissingCommaError, fileName, lineNumber);
				return ERROR;
			}
			if (status == 0)
			{
				status = 1;
			}
			length++;
			flagNum = 1;
			number[i] = *currChar;
			currChar = currChar + 1;
			i++;
		}
		else if (*currChar == '+' || *currChar == '-')
		{
			if (status != 0)
			{
				addError(AdditionalSymbolError, fileName, lineNumber);
				return ERROR;
			}
			else if (*currChar == '+')
			{
				status = 1;
			}
			else
			{
				status = 2;
			}
			length++;
			currChar++;
		}
		else if (*currChar == ' ' || *currChar == '\t')
		{
			flagWhiteSpace = 1;
			length++;
			currChar++;
		}
		else
		{
			addError(IllegalLabelError, fileName, lineNumber);
			return ERROR;
		}
	}
	if (*currChar == ',')
	{
		*str = *str + 1;
		if (flagNum == 1)
		{
			if (**str == '\0')
			{
				addError(IllegalDataFormat, fileName, lineNumber);
				return ERROR;
			}
		}
		else
		{
			addError(AdditionalSymbolError, fileName, lineNumber);
			return ERROR;
		}
	}
	number[i] = '\0';
	num = atoi(number);
	*str = *str + length;
	if (status == 1)
	{
		return num;
	}
	else
	{
		return num * (-1);
	}
}

/**
 * Check if the symbol already in the symbol table.
 * Return 1 if it exists, 0 otherwise.
 */
int symbolExist(char *symbolName)
{
	SymbolList *tempHead;
	tempHead = symbolHead;
	while (tempHead != NULL)
	{
		if (strcmp(tempHead->name, symbolName) == 0)
		{
			return 1;
		}
		else
		{
			tempHead = tempHead->next;
		}
	}
	return 0;
}

/**
 * check if the word is start of guidance sentence
 * return 1 if .data
 * return 2 if .string
 * return 3 if .extern
 * return 4 if .entry
 * return -1 if the word isn't start of guidance sentence
*/
int getGuidanceCode(char *word)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		if (strcmp(word, GUIDANCE[i]) == 0)
		{
			return i + 1;
		}
	}
	return -1;
}

/**
 * reset the string to zero
 */
void resetLine(char *line)
{
	int i;
	for (i = 0; i < LINE_LENGTH; i++)
	{
		line[i] = 0;
	}
}

/**
 * add a node to the end of the symbol list and update the tail to the end
 */
void addToSymbolList(char *name, int address, int isGuidance, int isExtern)
{
	SymbolList *tempSymbol;
	tempSymbol = (SymbolList *) malloc(sizeof(SymbolList));
	strcpy(tempSymbol->name, name);
	tempSymbol->address = address;
	tempSymbol->isGuidance = isGuidance;
	tempSymbol->isExtern = isExtern;
	strcpy(tempSymbol->filename, fileName);
	tempSymbol->next = NULL;
	if (symbolHead == NULL)
	{
		symbolHead = tempSymbol;
		symbolTail = symbolHead;
		return;
	}
	symbolTail->next = tempSymbol;
	symbolTail = tempSymbol;
}

/**
 * add a node to the end of the command list and update the tail to the end
 */
void
addToCommandList(int address, int code, int required_args, char *srcArg, char *destArg, int srcMethod, int destMethod,
                 char *label)
{
	CommandsList *tempCommand;
	tempCommand = (CommandsList *) malloc(sizeof(CommandsList));
	tempCommand->address = address;
	tempCommand->code = code;
	tempCommand->required_args = required_args;
	tempCommand->next = NULL;
	strcpy(tempCommand->operationLabel, label);
	strcpy(tempCommand->destArg, destArg);
	strcpy(tempCommand->srcArg, srcArg);
	tempCommand->destMethod = destMethod;
	tempCommand->srcMethod = srcMethod;
	strcpy(tempCommand->fileName, fileName);
	tempCommand->lineNumber = lineNumber;
	if (commandHead == NULL)
	{
		commandHead = tempCommand;
		commandTail = commandHead;
		return;
	}
	commandTail->next = tempCommand;
	commandTail = tempCommand;
}

/**
 * add a node to the end of the data list and update the tail to the end
 */
void addToDataList(int address, int code, int isString)
{
	DataList *tempData;
	tempData = (DataList *) malloc(sizeof(DataList));
	tempData->address = address;
	tempData->code = code;
	tempData->next = NULL;
	strcpy(tempData->fileName, fileName);
	tempData->lineNumber = lineNumber;
	if (dataHead == NULL)
	{
		dataHead = tempData;
		dataTail = dataHead;
		return;
	}
	dataTail->next = tempData;
	dataTail = tempData;
}

/**
 * check if label is legal
 * return 0 if illegal
 * return 1 if legal
 * return 2 if legal and end with char ':' (start of sentence)
 */
int isLegalLabel(char *label)
{
	char *tempLabel;
	int i;
	int length;
	length = (int) strlen(label);
	tempLabel = label;
	if (wordIsSave(label) == 1)
	{
		return ERROR;
	}
	for (i = 0; i < length; i++)
	{
		if (i == 0 && charIsAlpha(*tempLabel) == 0) /*first char is not alphabet*/
		{
			return ILLEGAL;
		}
		else if (i == length - 1 && *tempLabel == ':') /* if label is at start of sentence */
		{
			return 2; /* Is command line */
		}
		else if ((i == length - 1 && (charIsAlpha(*tempLabel) == 1 || charIsDigit(*tempLabel) == 1))
		         || *tempLabel == '\r' || *tempLabel == '\n') /* if last char is number or alphabet */
		{
			return LEGAL;
		}
		else if (charIsAlpha(*tempLabel) == 0 && charIsDigit(*tempLabel) == 0) /* if char is not a number or letter*/
		{
			return ILLEGAL;
		}
		else
		{
			tempLabel = tempLabel + 1;
		}
	}
	return ILLEGAL;
}

/**
 * check if the char is digit
 * return 1 if yes or 0 if not
 */
int charIsDigit(char ch)
{
	if (ch >= '0' && ch <= '9')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * check if the char is alphabet
 * return 1 if yes or 0 if not
 */
int charIsAlpha(char ch)
{
	if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * check if the word is saved (register, guidance or operation)
 * return 1 if the word is saved or 0 if not
 */


int wordIsSave(char *label)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		if (strcmp(label, REGISTER[i]) == 0) /* check if register */
		{
			return 1;
		}
	}
	for (i = 0; i < 4; i++)
	{
		if (strcmp(label, GUIDANCE[i]) == 0) /* check if guidance */
		{
			return 1;
		}
	}
	for (i = 0; i < 16; i++)
	{
		if (strcmp(label, COMMANDS[i]) == 0) /* check if register*/
		{
			return 1;
		}
	}
	return 0;
}

/**
 * promote the pointer to the next word (deny whitespace)
 * return pointer to the next word or NULL if not exist
 */
char *getNextWord(char *word)
{
	char delim[3];
	delim[0] = ' ';
	delim[1] = '\t';
	delim[2] = ',';
	word = strtok(NULL, delim);
	return word;
}

/**
 * promote the pointer to the start of the parameters
 */
char *getGuidanceLine(char *word)
{
	char delim[2];
	delim[0] = ' ';
	delim[1] = '\t';
	word = strtok(NULL, delim);
	return word;
}

/**
 * return 1 if word is last in line -1 otherwise
 */
int isLastWord(char *word)
{
	char delim[2];
	delim[0] = ' ';
	delim[1] = '\t';
	word = strtok(NULL, delim);
	if (word == NULL || strcmp(word," ")==0 || strcmp(word,"\t")==0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 * return the next arg
 */
char *getNextArg(char *word)
{
	char delim[3];
	delim[0] = ' ';
	delim[1] = '\t';
	delim[2] = ',';
	word = strtok(NULL, delim);
	return word;
}


/**
 * return addressing method
 */
int getAddressReallocationMethod(char *word)
{
	int length;
	length = strlen(word);
	if (word[length - 2] == '\r')
	{
		word[length - 2] = '\0';
	}
	else if (word[length - 1] == '\n')
	{
		word[length - 1] = '\0';
	}
	if (isDirectNumber(word) == TRUE)
	{
		return 0;
	}
	if (isLegalLabel(word) == TRUE)
	{
		return 1;
	}
	if (isRegisterAddress(word) == TRUE)
	{
		return 2;
	}
	if (isRegister(word) == TRUE)
	{
		return 3;
	}


	return ERROR;
}

/**
 Check for Immediate addressing method
 */
int isDirectNumber(char *word)
{
	char *tempWord;
	tempWord = word;
	if (*word == '#')
	{
		tempWord = tempWord + 1;
		if (isNumber(tempWord))
		{
			return 1;
		}
	}
	return ERROR;
}

/**
 * check if the word is number
 * return 1 if yes ERROR otherwise
 */
int isNumber(char *word)
{
	while (*word != ' ' && *word != '\t' && *word != '\n')
	{
		if (!charIsDigit(*word))
		{
			return ERROR;
		}
		word = word + 1;
	}
	return 1;
}

void initLists()
{
	commandHead = NULL;
	symbolHead = NULL;
	dataHead = NULL;
	errorHead = NULL;
}

void initCounters()
{
	dc = 0;
	ic = START_POSITION;
}


void readline_secondRun(char *line)
{
	char *word;
	char delim[3];
	char *symbol;
	int type;
	delim[0] = ' ';
	delim[1] = '\t';
	delim[2] = ',';
	word = strtok(line, delim);
	type = getGuidanceCode(word);
	if (isEOF(word) || shouldSkipLineSecondRun(word) == TRUE)
	{
		return;
	}
	else if (type != FALSE)
	{
		if (type == ENTRY)
		{
			symbol = getNextWord(word);
			if (symbol[strlen(symbol) - 2] == '\r')
			{
				symbol[strlen(symbol) - 2] = '\0';
			}
			else if (symbol[strlen(symbol) - 1] == '\n')
			{
				symbol[strlen(symbol) - 1] = '\0';
			}
			addEntryToSymbols(symbol, lineNumber);
			return;
		}
	}
}


void addEntryToSymbols(char *symbolName, int lineNum)
{
	SymbolList *symbol;
	symbol = getSymbol(symbolName);
	if (symbol == NULL)
	{
		addError(SymbolNotFoundError, fileName, lineNum);
	}
	else
	{
		symbol->isEntry = TRUE;
	}
}

int checkErrorList()
{
	if (errorHead != NULL)
	{
		return TRUE;
	}
	return FALSE;
}

int shouldSkipLineSecondRun(char *word)
{
	int type;
	type = getGuidanceCode(word);
	if (type != FALSE)
	{
		if (type == ENTRY)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

void updateSymbolList()
{
	SymbolList *curr;
	curr = symbolHead;
	while (curr != NULL)
	{
		if (curr->isGuidance == 1)
		{
			curr->address += ic;
		}
		curr = curr->next;
	}
}

void fixLabelName()
{
	SymbolList *curr;
	char *currLabel;
	curr = symbolHead;

	while (curr != NULL)
	{
		currLabel = curr->name;
		while (*currLabel != '\0')
		{
			if (*currLabel == ':' || *currLabel == '\r' || *currLabel == '\n')
			{
				*currLabel = '\0';
				break;
			}
			currLabel++;
		}
		curr = curr->next;
	}
}

void fixOperationLabel()
{
	CommandsList *curr;
	char *opLabel;
	curr = commandHead;

	while (curr != NULL)
	{
		opLabel = curr->operationLabel;
		while (*opLabel != '\0')
		{
			if (*opLabel == ':' || *opLabel == '\r' || *opLabel == '\n')
			{
				*opLabel = '\0';
				break;
			}
			opLabel++;
		}
		curr = curr->next;
	}
}

void updateDataAddress()
{
	DataList *curr;
	curr = dataHead;
	while (curr != NULL)
	{
		curr->address += ic;
		curr = curr->next;
	}
}
