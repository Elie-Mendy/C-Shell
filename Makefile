# definition de variables 
CC = gcc
CFLAGS = -Wall main.c
LIB = -L/usr/include -lreadline
EXEC = bash-shell
SRC = main.c


# regle all
all : $(EXEC)


# creation de l'executable
# 	gcc -Wall menshell.c -L/usr/include -lreadline -o menshell
$(EXEC) : $(SRC) 
	$(CC) $(CFLAGS) $(LIB) -o $(EXEC)

