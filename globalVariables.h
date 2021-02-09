#ifndef MAMAN14_GLOBALVARIABLES_H
#define MAMAN14_GLOBALVARIABLES_H

#include "dataStructures.h"

CommandsList *   commandHead,    * commandTail;
DataList *       dataHead,       * dataTail;
SymbolList *     symbolHead,     * symbolTail;
ErrorList *      errorHead,      * errorTail;
int ic;
int dc;
int lineNumber;
int secondRunErrors;

char * fileName;
char * COMMANDS[16];
char * GUIDANCE[4];
char * REGISTER[8];
char * REGISTER_ADDRESS[8];

#endif /* MAMAN14_GLOBALVARIABLES_H */

