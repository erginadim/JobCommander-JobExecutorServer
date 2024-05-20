SOURCE = communication.c jobCommander.c jobExecutorserver.c list.c main.c
OBJS = communication.o jobCommander.o jobExecutorserver.o list.o main.o
HEADER = headers.h list.h communication.h
OUT = jobCommander
FILES = jobExecutorServer fifoES fifoJC
CC = gcc
FLAGS = -c

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT)

main.o : main.c
	$(CC) $(FLAGS) main.c

communication.o : communication.c
	$(CC) $(FLAGS) communication.c

jobCommander.o : jobCommander.c
	$(CC) $(FLAGS) jobCommander.c

jobExecutorServer.o : jobExecutorserver.c
	$(CC) $(FLAGS) jobExecutorserver.c

list.o : list.c
	$(CC) $(FLAGS) list.c

build : $(SOURCE)
	$(CC) -o $(OUT) $(SOURCE)

clean : 
	rm -f $(OBJS) $(OUT) $(FILES)

run : $(OUT)
	./$(OUT) 


rebuild : clean build