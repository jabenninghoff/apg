# You can modify CC variable if you have compiler other than GCC
# But the code was designed and tested with GCC
CC = gcc

# compilation flags
# You should comment the line below for AIX+native cc
FLAGS = -Wall

# libraries
LIBS = -lcrypt

# DO NOT EDIT THE LINE BELOW !!!
CRYPTED_PASS = APG_DONOTUSE_CRYPT
# Coment this if you do not want to use crypted passwords output
CRYPTED_PASS = APG_USE_CRYPT

# Install dirs
INSTALL_PREFIX = /usr/local
APG_BIN_DIR = /bin
APG_MAN_DIR = /man/man1
APGD_BIN_DIR = /sbin
APGD_MAN_DIR = /man/man8

# Find group ID for user root
FIND_GROUP = `grep '^root:' /etc/passwd | awk -F: '{ print $$4 }'`

####################################################################
# If you plan to install APG daemon you should look at lines below #
####################################################################

# Uncoment NOTHING for FreeBSD
#

# Uncoment line below for LINUX
#CS_LIBS = -lnsl

# Uncoment line below for Solaris
#CS_LIBS = -lnsl -lsocket

# ====== YOU DO NOT NEED TO MODIFY ANYTHING BELOW THIS LINE ======

PROGNAME = apg
CS_PROGNAME = apgd
SOURCES = rnd.c ./cast/cast.c pronpass.c randpass.c restrict.c errors.c apg.c
HEADERS = owntypes.h pronpass.h randpass.h restrict.h errs.h rnd.h ./cast/cast.h ./cast/cast_sboxes.h
OBJECTS = rnd.o ./cast/cast.o pronpass.o randpass.o restrict.o apg.o errors.o

all: cliserv standalone

cygwin: standalone

cliserv: ${SOURCES} ${HEADERS}
	${CC} ${FLAGS} ${CS_LIBS} -DCLISERV -o ${CS_PROGNAME} ${SOURCES}

standalone: ${SOURCES} ${HEADERS}
	${CC} ${FLAGS} ${LIBS} -D${CRYPTED_PASS} -o ${PROGNAME} ${SOURCES}

strip:
	strip ${PROGNAME}
	strip ${CS_PROGNAME}

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

install-cygwin:
	if test -x ./apg.exe; then \
./mkinstalldirs ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./mkinstalldirs ${INSTALL_PREFIX}${APG_MAN_DIR}; \
./install-sh -c -m 0755 ./apg.exe ${INSTALL_PREFIX}${APG_BIN_DIR}; \
./install-sh -c -m 0444 ./doc/man/apg.1 ${INSTALL_PREFIX}${APG_MAN_DIR}; \
fi

clean:
	rm -f ${CS_PROGNAME} ${PROGNAME} ${OBJECTS} core*
