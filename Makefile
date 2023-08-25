# definition de variables 
CC = gcc
CFLAGS = -Wall menshell.c
LIB = -L/usr/include -lreadline
EXEC = menshell
SRC = menshell.c


# regle all
all : $(EXEC)


# creation de l'executable
# 	gcc -Wall menshell.c -L/usr/include -lreadline -o menshell
$(EXEC) : $(SRC) 
	$(CC) $(CFLAGS) $(LIB) -o $(EXEC)

