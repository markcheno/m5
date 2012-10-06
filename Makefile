#############################################################################
# Makefile for building m3
#############################################################################

####### Compiler, tools and options

CC	= 	gcc
CFLAGS	=	-pipe -Wall -W -O3
#CFLAGS	=	-D_POSIX_SOURCE -Wall -ansi -pedantic -g
INCPATH	=	
LINK	=	gcc
LFLAGS	=	
LIBS	=	
TAR	=	tar -cvzf

####### Files

HEADERS =	scanner.h \
		ast.h \
		code_pseudo.h \
		parser.h \
		symbol.h \
		tokens.h \
		type.h \
		error.h

SOURCES =	scanner.l \
		ast.c \
		code_pseudo.c \
		main.c \
		parser.c \
		symbol.c \
		type.c \
		error.c

OBJECTS =	scanner.o \
		ast.o \
		code_pseudo.o \
		main.o \
		parser.o \
		symbol.o \
		type.o \
		error.o
		
DIST	=	Makefile docs test asm

TARGET	=	m5

####### Implicit rules

.SUFFIXES: .bnf .c .h

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

archive:
	$(TAR) $(TARGET)-$(shell date +'%y%m%d%H').tar.gz $(SOURCES) $(HEADERS) $(DIST)

dist:
	$(TAR) $(TARGET).tar.gz $(SOURCES) $(HEADERS) $(DIST)

clean:
	-rm -f $(OBJECTS) $(TARGET).h $(TARGET)
	-rm -f *~ core *.lis mcc.tmp scanner.c

####### Compile

scanner.c:	scanner.l
		rm -f $@
		lex -t scanner.l >$@

ast.o:  	ast.c \
		ast.h \
		tokens.h \
		symbol.h \
		type.h

code_pseudo.o: 	code_pseudo.c \
		code_pseudo.h \
		tokens.h \
		ast.h \
		symbol.h \
		type.h \
		error.h

parser.o:	parser.c \
		parser.h \
		scanner.h \
		symbol.h \
		ast.h \
		type.h \
		code_pseudo.h 

$(TARGET).o: 	main.c \
		parser.h \
		code_pseudo.h

symbol.o:  	symbol.c \
		symbol.h \
		type.h

type.o: 	type.c \
		type.h \
		symbol.h \
		error.h

error.o: 	error.c \
		error.h \
		scanner.h
