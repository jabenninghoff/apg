##################################################################
# You can modify CC variable if you have compiler other than GCC
# But the code was designed and tested with GCC
CC = gcc

##################################################################
# Compilation flags
# You should comment the line below for AIX+native cc
FLAGS = -Wall

##################################################################
# Libraries
#
# You should comment the line below ('LIBS= -lcrypt')for QNX RTP
# 6.1.0, OpenBSD 2.8 and above, WIN32 (+MinGW)
LIBS = -lcrypt
LIBM = -lm
# Use lines below for cygwin
# LIBS = 
# LIBM =

##################################################################
# Support for crypted passwords
#
# DO NOT EDIT THE LINE BELOW !!!
CRYPTED_PASS = APG_DONOTUSE_CRYPT
# Coment this if you do not want to use crypted passwords output
# or trying to build programm for win32
CRYPTED_PASS = APG_USE_CRYPT

##################################################################
# Support for ANSI X9.17/SHA1 PRNG
#
# DO NOT EDIT THE LINE BELOW !!!
USE_SHA = APG_USE_SHA
# Coment this if you want to use PRNG X9.17 with SHA-1
USE_SHA = APG_DONOTUSE_SHA

##################################################################
# Directories
# Install dirs
INSTALL_PREFIX = /usr/local
APG_BIN_DIR = /bin
APG_MAN_DIR = /man/man1
APGD_BIN_DIR = /sbin
APGD_MAN_DIR = /man/man8

####################################################################
# If you plan to install APG daemon you should look at lines below #
####################################################################

####################################################################
# FreeBSD
#
# Uncoment NOTHING for FreeBSD
#

####################################################################
# Linux
#
# Uncoment line below for LINUX
#CS_LIBS = -lnsl

####################################################################
# Solaris
#
# Uncoment line below for Solaris
#CS_LIBS = -lnsl -lsocket

####################################################################
# QNX RTP 6.1.0
#
# Uncoment line below for QNX RTP 6.1.0
#CS_LIBS = -lsocket

# ====== YOU DO NOT NEED TO MODIFY ANYTHING BELOW THIS LINE ======
# Find group ID for user root
FIND_GROUP = `grep '^root:' /etc/passwd | awk -F: '{ print $$4 }'`

PROGNAME = apg
CS_PROGNAME = apgd
BFM_PROGNAME = apgbfm
BFM_SOURCES = apgbfm.c bloom.c sha/sha.c errors.c getopt.c convert.c
SOURCES = bloom.c ./sha/sha.c ./cast/cast.c rnd.c pronpass.c \
randpass.c restrict.c errors.c apg.c getopt.c convert.c
HEADERS = owntypes.h pronpass.h randpass.h restrict.h errs.h rnd.h \
./cast/cast.h ./cast/cast_sboxes.h getopt.h convert.h
OBJECTS = rnd.o ./cast/cast.o pronpass.o randpass.o restrict.o apg.o errors.o

standalone: apg apgbfm

all: cliserv standalone

cliserv: apgd apgbfm 

cygwin: standalone

apg:
	${CC} ${FLAGS} -D${CRYPTED_PASS} -D${USE_SHA} -o ${PROGNAME} ${SOURCES} ${LIBS} ${LIBM}

apgd:
	${CC} ${FLAGS} -DCLISERV -D${USE_SHA} -o ${CS_PROGNAME} ${SOURCES} ${CS_LIBS} ${LIBM}

apgbfm:
	${CC} ${FLAGS} -DAPGBFM -o ${BFM_PROGNAME} ${BFM_SOURCES} ${LIBM}

strip:
	strip ${PROGNAME}
	strip ${CS_PROGNAME}
	strip ${BFM_PROGNAME}

install:
	if test -x ./apg; then \
./mkinstalldirs ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./mkinstalldirs ${INSTALL_PREFIX}${APG_MAN_DIR}; \
./install-sh -c -m 0755 -o root -g ${FIND_GROUP} ./apg ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./install-sh -c -m 0444 ./doc/man/apg.1 ${INSTALL_PREFIX}${APG_MAN_DIR}; \
fi
	if test -x ./apgd; then \
./mkinstalldirs ${INSTALL_PREFIX}${APGD_BIN_DIR}; \
./mkinstalldirs ${INSTALL_PREFIX}${APGD_MAN_DIR}; \
./install-sh -c -m 0755 -o root -g ${FIND_GROUP} ./apgd ${INSTALL_PREFIX}${APGD_BIN_DIR}; \
./install-sh -c -m 0444 ./doc/man/apgd.8 ${INSTALL_PREFIX}${APGD_MAN_DIR}; \
fi
	if test -x ./apgbfm; then \
./mkinstalldirs ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./mkinstalldirs ${INSTALL_PREFIX}${APG_MAN_DIR}; \
./install-sh -c -m 0755 -o root -g ${FIND_GROUP} ./apgbfm ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./install-sh -c -m 0444 ./doc/man/apgbfm.1 ${INSTALL_PREFIX}${APG_MAN_DIR}; \
fi

install-cygwin:
	if test -x ./apg.exe; then \
./mkinstalldirs ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./mkinstalldirs ${INSTALL_PREFIX}${APG_MAN_DIR}; \
./install-sh -c -m 0755 ./apg.exe ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./install-sh -c -m 0444 ./doc/man/apg.1 ${INSTALL_PREFIX}${APG_MAN_DIR}; \
fi
	if test -x ./apgbfm.exe; then \
./mkinstalldirs ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./mkinstalldirs ${INSTALL_PREFIX}${APG_MAN_DIR}; \
./install-sh -c -m 0755 ./apgbfm.exe ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./install-sh -c -m 0444 ./doc/man/apgbfm.1 ${INSTALL_PREFIX}${APG_MAN_DIR}; \
fi

clean:
	rm -f ${CS_PROGNAME} ${PROGNAME} ${BFM_PROGNAME} ${OBJECTS} *core*
