#hello

CC = gcc

DIR_INC = ./include 
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.c)

OBJS = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = ${DIR_BIN}/mymplayer

CFLAGS = -Wall -I${DIR_INC}
$(TARGET):$(OBJS)
	$(CC) $(OBJS) -lpthread -o $@

${DIR_OBJ}/%.o:${DIR_SRC}/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
#debug to list OBJS and SRC
test:
	@echo "OBJ Files:"
	@echo $(OBJS)
	@echo "SRC files:"
	@echo $(SRC)


clean:
	rm -f $(TARGET) $(OBJS)
.PHONY:clean

