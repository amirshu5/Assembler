#include "general.h"

void getARE(int addressMethod, int isExtern, char *filename, int linenumber, int binary[])
{
	int i;
	int start, end;
	start = 12;
	end = 14;
	for (i = start; i <= end; i++)
	{
		binary[i] = 0;
	}

	if (addressMethod == IMMEDIATE_ADDRESSING || addressMethod == INDIRECT_REGISTER_ADDRESSING
	    || addressMethod == DIRECT_REGISTER_ADDRESSING || addressMethod == OPERATION_WORD)
	{
		binary[start + 0] = 1;
	}
	else if (addressMethod == DIRECT_ADDRESSING)
	{
		if (isExtern == TRUE)
		{
			binary[start + 2] = 1;
		}
		else
		{
			binary[start + 1] = 1;
		}
	}
	else
	{
		secondRunErrors = TRUE;
		addError(IllegalAddressingMethod, filename, linenumber);
	}
}

void getAddressingMethodInBinary(int method, char *filename, int linNumber, int binary[], int src)
{
	int i, start, end;

	if (src == TRUE)
	{
		start = 4;
		end = 7;
	}
	else
	{
		start = 8;
		end = 11;
	}

	for (i = start; i <= end; i++)
	{
		binary[i] = 0;
	}

	switch (method)
	{
		case 0:
			binary[end] = 1;
			break;
		case 1:
			binary[end - 1] = 1;
			break;
		case 2:
			binary[start + 1] = 1;
			break;
		case 3:
			binary[start] = 1;
			break;
		case NOT_USED_INT:
			break;

		default:
			addError(IllegalAddressingMethod, filename, lineNumber);
			secondRunErrors = TRUE;
			break;
	}
}

void translateOpCode(int decimal, int binary[])
{
	convertToBinary(decimal, 0, 3, binary, FALSE);
}

void convertToBinary(int decimal, int start, int end, int binary[], int isData)
{
	int i, isNegative;

	isNegative = (decimal < 0 ? TRUE : FALSE);
	if (decimal < 0)
	{
		decimal = abs(decimal);
	}
	i = end;
	while (decimal > 0)
	{
		binary[i] = decimal % 2;
		decimal = decimal / 2;
		i--;
	}
	/* add zeros to fill (end-start + 1) bits */
	while (i >= start)
	{
		binary[i] = 0;
		i--;
	}


	if (isNegative == TRUE)
	{
		convertToTwosComplement(binary, isData);
	}
}

void convertToTwosComplement(int binary[], int isData)
{
	int i, max, carry;
	max = (isData == TRUE ? WORD_LENGTH : BITS_FOR_ARG_VALUE);
	/* get 1's complement */
	for (i = 0; i < max; i++)
	{
		if (binary[i] == 1)
		{
			binary[i] = 0;
		}
		else
		{
			binary[i] = 1;
		}
	}
	/* add one to get 2's complement */
	carry = 1;
	i = max - 1;
	while (i >= 0 && carry == 1)
	{
		if (binary[i] == 0)
		{
			binary[i] = 1;
			carry = 0;
		}
		else
		{
			binary[i] = 0;
		}
		i--;
	}
}

void convertToOctal(int binary[], int octal[])
{
	int i, j, k, sum;
	j = 0;
	sum = 0;
	k = 4;
	for (i = WORD_LENGTH - 1; i >= 0; i--)
	{
		sum += binary[i] * pow(2, j);
		j++;
		if (j == 3)
		{
			octal[k] = sum;
			k--;
			j = 0;
			sum = 0;
		}
	}
}

int getNumberFromArg(char *arg, CommandsList *node)
{
	int num;
	char *localArg;
	localArg = arg + 1;
	num = atoi(localArg);
	if (num == 0)
	{
		if (strcmp(localArg, "0") != 0)
		{
			addError(NumberConversionFailed, node->fileName, node->lineNumber);
			secondRunErrors = TRUE;
		}
	}
	return num;


}

int getRegisterNumber(char *arg, int direct)
{
	int i, num;
	num = FALSE; /* If register name is illegal */
	if (direct == TRUE)
	{
		for (i = 0; i < NUM_OF_REGISTERS; i++)
		{
			if (strcmp(arg, REGISTER[i]) == 0)
			{
				num = i;
			}
		}
	}
	else
	{
		for (i = 0; i < NUM_OF_REGISTERS; i++)
		{
			if (strcmp(arg, REGISTER_ADDRESS[i]) == 0)
			{
				num = i;
			}
		}
	}

	return num;
}

void getBasename(char *orig, char *resolved)
{
	int i, j;
	int gotName;
	i = 0;
	j = 0;
	gotName = FALSE;

	while (orig[i] != '\0' && orig[i] != '\n')
	{
		if ((orig[i] == '.') && (gotName == TRUE))
		{
			break;
		}
		else if (orig[i] == '\\' || orig[i] == '/' || orig[i] == '.')
		{
			i++;

		}
		else
		{
			resolved[j] = orig[i];
			i++;
			j++;
			gotName = TRUE;
		}

	}
	resolved[j++] = '\0';

}

void printData(FILE *fp)
{
	DataList *current;
	int binary[WORD_LENGTH];
	int octal[OCTAL_LENGTH];
	current = dataHead;
	while (current != NULL)
	{
		convertToBinary(current->code, 0, WORD_LENGTH - 1, binary, TRUE);
		convertToOctal(binary, octal);
		writeObjLine(fp, octal, current->address);
		current = current->next;
	}
}

void printEntry(FILE *fp)
{
	SymbolList *current;
	current = symbolHead;
	while (current != NULL)
	{
		if (current->isEntry == TRUE)
		{
			writeEntLine(fp, current->name, current->address);
		}
		current = current->next;
	}
}

void finalize(FILE *obj, FILE *ent, FILE *ext, char *objName, char *extName, char *entName, int hasErrors)
{
	fclose(obj);
	fclose(ent);
	fclose(ext);
	if (hasErrors == TRUE)
	{
		remove(objName);
		remove(extName);
		remove(entName);
	}
}

void writeObjLine(FILE *fp, int octal[], int address)
{
	int i;
	fprintf(fp, "%04d\t", address);
	for (i = 0; i < 5; i++)
	{
		fprintf(fp, "%d", octal[i]);
	}
	fprintf(fp, "\n");
}

void writeExtLine(FILE *fp, char *name, int address)
{
	fprintf(fp, "%-10s\t%04d\n", name, address);
}

void writeEntLine(FILE *fp, char *name, int address)
{
	fprintf(fp, "%-10s\t%d\n", name, address);
}

int handleArgument(CommandsList *node, int isSrcArg, FILE *obj, FILE *ext, int twoRegisters, int hasTwoArgs)
{
	int binary[WORD_LENGTH];
	int octal[OCTAL_LENGTH];
	int method;
	int argValue; /*stores either direct number or register number */
	char *arg;
	char *filename;
	int linenumber;
	int address;
	SymbolList *symbol;

	filename = node->fileName;
	linenumber = node->lineNumber;

	if (isSrcArg == TRUE)
	{
		method = node->srcMethod;
		arg = node->srcArg;
		address = node->address + 1;
	}
	else
	{
		method = node->destMethod;
		arg = node->destArg;
		address = node->address + 2;
	}

	if (twoRegisters == TRUE || hasTwoArgs == FALSE)
	{
		address = node->address + 1;
	}

	switch (method)
	{
		case IMMEDIATE_ADDRESSING:
			argValue = getNumberFromArg(arg, node);
			convertToBinary(argValue, 0, BITS_FOR_ARG_VALUE - 1, binary, FALSE);
			getARE(method, FALSE, filename, linenumber, binary);
			convertToOctal(binary, octal);
			break;

		case DIRECT_ADDRESSING:
			symbol = getSymbol(arg);
			if (symbol == NULL)
			{
				addError(SymbolNotFoundError, filename, linenumber);
				return ERROR;
			}

			if (symbol->isExtern == TRUE)
			{
				convertToBinary(0, 0, BITS_FOR_ARG_VALUE - 1, binary, FALSE);
				getARE(method, TRUE, filename, linenumber, binary);
				convertToOctal(binary, octal);
				writeExtLine(ext, symbol->name, address);
			}
			else
			{
				convertToBinary(symbol->address, 0, BITS_FOR_ARG_VALUE - 1, binary, FALSE);
				getARE(method, FALSE, filename, linenumber, binary);
				convertToOctal(binary, octal);
				/* entry file is written later */
			}
			break;

		case INDIRECT_REGISTER_ADDRESSING:
			argValue = getRegisterNumber(arg, FALSE);
			if (isSrcArg == TRUE || twoRegisters != TRUE)
			{
				convertToBinary(0, 0, BITS_FOR_ARG_VALUE - 1, binary, FALSE); /* initialize all to zero */
			}

			if (isSrcArg == TRUE)
			{
				convertToBinary(argValue, SRC_REG_START, SRC_REG_END, binary, FALSE);
			}
			else
			{
				convertToBinary(argValue, DEST_REG_START, DEST_REG_END, binary, FALSE);
			}

			getARE(method, FALSE, filename, linenumber, binary);
			convertToOctal(binary, octal);
			break;

		case DIRECT_REGISTER_ADDRESSING:
			argValue = getRegisterNumber(arg, TRUE);
			if (isSrcArg == TRUE || twoRegisters != TRUE)
			{
				convertToBinary(0, 0, BITS_FOR_ARG_VALUE - 1, binary, FALSE); /* initialize all to zero */
			}
			if (isSrcArg == TRUE)
			{
				convertToBinary(argValue, SRC_REG_START, SRC_REG_END, binary, FALSE);
			}
			else
			{
				convertToBinary(argValue, DEST_REG_START, DEST_REG_END, binary, FALSE);
			}
			getARE(method, FALSE, filename, linenumber, binary);
			convertToOctal(binary, octal);
			break;

		default:
			addError(IllegalAddressingMethod, filename, lineNumber);
			return ERROR;
	}

	if (twoRegisters == TRUE)
	{
		if (isSrcArg == FALSE)
		{
			writeObjLine(obj, octal, address);
		}
	}
	else
	{
		writeObjLine(obj, octal, address);
	}


	return 0;
}

int checkIfTwoRegisters(CommandsList *node)
{
	int src;
	int dest;
	src = node->srcMethod;
	dest = node->destMethod;
	if ((src == DIRECT_REGISTER_ADDRESSING || src == INDIRECT_REGISTER_ADDRESSING)
	    && (dest == DIRECT_REGISTER_ADDRESSING || dest == INDIRECT_REGISTER_ADDRESSING))
	{
		return TRUE;
	}
	return FALSE;


}