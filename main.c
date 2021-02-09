#include <stdio.h>

#include "utilities.h"
#include "errors.h"
#include "globalVariables.h"
#include "constants.h"
#include "writers.h"

int main(int argc, char *argv[])
{
	FILE *fp;
	char line[LINE_LENGTH];
	int i, j;
	initLists();
	initCounters();
	resetLine(line);
	for (i = 1; i < argc; i++)
	{
		fileName = argv[i];
		fp = fopen(fileName, "r");
		rewind(fp); /* used as safety to make sure pointer is at top of file */
		for (j = 1; j <= 2; j++)
		{
			lineNumber = 1;
			if (fp == NULL)
			{
				printf("%s %s\n", IOError, argv[i]);
			}
			else
			{
				while (fgets(line, LINE_LENGTH, fp) != NULL)
				{
					if (j == 1)
					{
						readLine_firstRun(line);
						lineNumber++;
					}
					else
					{
						readline_secondRun(line);
						lineNumber++;
					}
				}
				if (j == 1)
				{
					updateSymbolList();
					fixLabelName();
					fixOperationLabel();
					updateDataAddress();
				}
				else
				{
					writeOutputFiles();

				}
				rewind(fp);
			}
		}
		fclose(fp);
	}
	printErrorList();
	return 0;
}
