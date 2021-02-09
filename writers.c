#include "general.h"

void writeOutputFiles()
{
	char baseFileName[FILENAME_MAX];
	char objFileName[FILENAME_MAX];
	char extFileName[FILENAME_MAX];
	char entFileName[FILENAME_MAX];

	CommandsList *currentNode;
	SymbolList *currentSymbol;

	FILE *objFile, *entFile, *extFile;
	

	int binaryString[WORD_LENGTH];
	int octal[OCTAL_LENGTH];

	char *filename;
	int linenumber;

	int twoRegisters;
	int temp;

	secondRunErrors = FALSE;

	getBasename(fileName, baseFileName);

	strcpy(objFileName, baseFileName);
	strcat(objFileName, ".ob");

	strcpy(extFileName, baseFileName);
	strcat(extFileName, ".ext");

	strcpy(entFileName, baseFileName);
	strcat(entFileName, ".ent");


	objFile = fopen(objFileName, "w");
	entFile = fopen(entFileName, "w");
	extFile = fopen(extFileName, "w");

	fprintf(objFile, "\t%d %d\n", ic - START_POSITION, dc);

	currentNode = commandHead;
	while (currentNode != NULL)
	{
		/* Check if command is a label */
		if (strcmp(currentNode->operationLabel, NO_LABEL) != 0)
		{
			currentSymbol = getSymbol(currentNode->operationLabel);
			if (currentSymbol == NULL)
			{
				addError(SymbolNotFoundError, currentNode->fileName, currentNode->lineNumber);
				secondRunErrors = TRUE;
			}
			else if (currentSymbol->isExtern == TRUE)
			{
				currentSymbol->address = currentNode->address;
			}
		}

		filename = currentNode->fileName;
		linenumber = currentNode->lineNumber;

		/* write the instruction line to the ob.as file */
		translateOpCode(currentNode->code, binaryString);
		getAddressingMethodInBinary(currentNode->srcMethod, filename, linenumber, binaryString, TRUE);
		getAddressingMethodInBinary(currentNode->destMethod, filename, linenumber, binaryString, FALSE);
		getARE(OPERATION_WORD, FALSE, currentNode->fileName, currentNode->lineNumber, binaryString);
		convertToOctal(binaryString, octal);
		writeObjLine(objFile, octal, currentNode->address);

		switch (currentNode->required_args)
		{
			case 0:
				/* Already treated above in writing the label (if exists) and the instruction */
				break;
			case 1: /* instructions with one argument */
				temp = handleArgument(currentNode, FALSE, objFile, extFile, FALSE, FALSE);
				secondRunErrors = (temp == ERROR ? TRUE : FALSE);
				break;

			case 2: /*instructions with two arguments */
				twoRegisters = checkIfTwoRegisters(currentNode);
				temp = handleArgument(currentNode, TRUE, objFile, extFile, twoRegisters, TRUE);
				secondRunErrors = (temp == ERROR ? TRUE : FALSE);
				temp = handleArgument(currentNode, FALSE, objFile, extFile, twoRegisters, TRUE);
				secondRunErrors = (temp == ERROR ? TRUE : FALSE);
				break;
		}
		currentNode = currentNode->next;
	}

	printData(objFile);
	printEntry(entFile);
	finalize(objFile, entFile, extFile, objFileName, extFileName, entFileName, secondRunErrors);

}

void printErrorList()
{
	ErrorList *currentError;
	currentError = errorHead;
	if (currentError != NULL)
	{
		printf("Assembly failed: errors found during parsing. \n");
	}

	while (currentError != NULL)
	{
		printf("%s found in %s:%d\n", currentError->errorName, currentError->fileName, currentError->lineNum);
		currentError = currentError->next;
	}
}
