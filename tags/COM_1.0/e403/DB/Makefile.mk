# -----------------------------------------------------------------------------------------------
# Makefile for building the LOS ANGELES TCN database
#
# 23.lug.2004            ver 1.00                  Schiavo
# -----------------------------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------------
# Base directories
# -----------------------------------------------------------------------------------------------

DIR_MASTER = C:\MASTER
DIR_SW     = C:\SW

# -----------------------------------------------------------------------------------------------
# Used directories
# -----------------------------------------------------------------------------------------------

DIR_LIB_BASE = $(DIR_SW)\COM\LIBS
DIR_LIB_INCS = $(DIR_LIB_BASE)\INC
DIR_LIB_LIBS = $(DIR_LIB_BASE)\LIB
DIR_LIB_SRCS = $(DIR_LIB_BASE)\SRC

DIR_DBS  = $(DIR_SW)\E403\DB
DIR_INCS = $(DIR_DBS)\SRC
DIR_SRCS = $(DIR_DBS)\SRC
DIR_OBJS = $(DIR_DBS)\OBJ


# -----------------------------------------------------------------------------------------------
# Set the directory where to store the produced objects and libraries
# -----------------------------------------------------------------------------------------------

.SOURCE.obj: $(DIR_OBJS)
.SOURCE.lib: $(DIR_OBJS)
.SOURCE.x:   $(DIR_OBJS)


# -----------------------------------------------------------------------------------------------
# *** This is the only section you need to change for a new project ***
# Rules to build the project files. In this section you >must< define the following macros:
# PROJECT        = project name
# DEPEND         = list of extra dependencies that force the .x rebuilding
# CC_EXTRA_FLAGS = extra flags to define for the C compilation
# OBJECTS        = list of objects to build
# -----------------------------------------------------------------------------------------------

PROJECT = dbsE403

#DEPEND  = $(DIR_LIB_LIBS)\com_tcn.lib
DEPEND  = $(DIR_LIB_LIBS)\com_mvb.lib

CC_EXTRA_FLAGS = -DATR_WTB



OBJECTS = \
        tcmsncdb.obj

HEADERS = \
        $(DIR_DBS)\SRC\tcmsncdb.h \
        $(DIR_DBS)\SRC\uic2ncdb.h \
        $(DIR_DBS)\SRC\gtw_ncdb.h



all: $(PROJECT).x

%.obj: $(DIR_SRCS)\%.c '$(HEADERS:t"' '")'

#.IF $(TARGET) == GWE403
	$(CC) -d $(CC_FLAGS_FILE) -o$@ $<
#.ELSE
#	$(CC) -d $(CC_FLAGS_FILE) -o$@ -DNO_GTWS $<
#.END

# -----------------------------------------------------------------------------------------------
# Rules to build the project executable
# -----------------------------------------------------------------------------------------------

$(PROJECT).x: $(PROJECT).lib

$(PROJECT).lib: $(OBJECTS)
	del $(DIR_OBJS)\$(PROJECT).lib
	$(LIB) < $(LIB_CMD_FILE)


# -----------------------------------------------------------------------------------------------
# C compiler definitions
# -----------------------------------------------------------------------------------------------

CC            = MCC68K

CC_FLAGS      = -p68360 -g -Xp -Za2 -c -U_SIZE_T -DMC68360 -DM68K -D__READY_EXTENSIONS__ \
                -Gf -O -Ob -Oe -Ot -zc \
                -J$(DIR_MASTER)/target/include -J$(DIR_MASTER)/target/device/include \
				-I$(DIR_LIB_INCS:t" -I") -I$(DIR_INCS:t" -I") -I$(DIR_SRCS:t" -I") -I@

CC_FLAGS_FILE := $(mktmp $(CC_FLAGS) $(CC_EXTRA_FLAGS)\n)



# -----------------------------------------------------------------------------------------------
# Librarian definitions
# -----------------------------------------------------------------------------------------------

LIB           = LIB68K

XXX = $(DIR_OBJS)/{$(OBJECTS)}
LIB_CMD_FILE  = $(mktmp CREATE $(DIR_OBJS)/$(PROJECT).lib\nADDMOD $(XXX:t"\n ADDMOD ")\nSAVE\nEND\n)


# -----------------------------------------------------------------------------------------------

