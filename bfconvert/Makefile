##################################################################
# You can modify CC variable if you have compiler other than GCC
# But the code was designed and tested with GCC
CC = gcc

##################################################################
# Compilation flags
# You should comment the line below for AIX+native cc
FLAGS = -Wall

ECHO = echo
PROGNAME = bfconvert
SOURCES  = bfconvert.c

all:
	${CC} ${FLAGS} -o ${PROGNAME} ${SOURCES}

strip:
	strip ${PROGNAME}

install:
	@${ECHO} "**********************************************"
	@${ECHO} "* This program shold be used to convert your *"  
	@${ECHO} "* filters once. So if you want to install    *"
	@${ECHO} "* this program you have to do it manualy :-) *"
	@${ECHO} "**********************************************"

clean:
	rm -f ${PROGNAME} *.o *core
