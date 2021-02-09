#ifndef MAMAN14_WRITERSUTILS_H
#define MAMAN14_WRITERSUTILS_H

void getARE(int addressMethod, int isExtern, char *filename, int linenumber, int binary[]);
void convertToBinary(int decimal, int start, int end, int binary[], int isData);
void convertToOctal(int binary[], int octal[]);
void getAddressingMethodInBinary(int method, char *filename, int linNumber, int binary[], int src);
void convertToTwosComplement(int binary[], int isData);
int getNumberFromArg(char *arg, CommandsList *node);
int getRegisterNumber(char *arg, int direct);
void getBasename(char *orig, char *resolved);
void printData(FILE *fp);
void printEntry(FILE *fp);
void finalize(FILE *obj, FILE *ent, FILE *ext, char *objName, char *extName, char *entName, int hasErrors);
void translateOpCode(int decimal, int binary[]);
void writeObjLine(FILE *fp, int octal[], int address);
void writeExtLine(FILE *fp, char *name, int address);
void writeEntLine(FILE *fp, char *name, int address);
int handleArgument(CommandsList *node, int isSrcArg, FILE *obj, FILE *ext, int twoRegisters, int hasTwoArgs);
int checkIfTwoRegisters(CommandsList *node);

#endif /*MAMAN14_WRITERSUTILS_H*/

