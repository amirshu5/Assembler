maman14 :dataStructures.o globalVariables.o main.o utilities.o writers.o writersUtils.o
	gcc -g -Wall -ansi -pedantic dataStructures.o globalVariables.o main.o utilities.o writers.o writersUtils.o -o maman14 -lm

dataStructures.o : dataStructures.c general.h
	gcc -c -Wall -ansi -pedantic dataStructures.c -o dataStructures.o -lm
globalVariables.o: globalVariables.c 
	gcc -c -Wall -ansi -pedantic globalVariables.c -o globalVariables.o -lm

main.o : main.c utilities.h errors.h globalVariables.h constants.h writers.h
	gcc -c -Wall -ansi -pedantic main.c -o main.o

utilities.o : utilities.c general.h
	gcc -c -Wall -ansi -pedantic utilities.c -o utilities.o -lm

writers.o : writers.c general.h
	gcc -c -Wall -ansi -pedantic writers.c -o writers.o -lm

writersUtils.o : writersUtils.c general.h
	gcc -c -Wall -ansi -pedantic writersUtils.c -o writersUtils.o -lm
