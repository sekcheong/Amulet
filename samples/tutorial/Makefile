##  This Makefile is designed to be used on unix platforms only
##    For Windows/MSVC++ platforms use tutorial.mak
##    For Macintosh/CodeWarrior platforms use the project
##	amulet/bin/MAC/samples/tutorial***.proj.bin
##
##  Set your AMULET_DIR environment variable to the top-level directory for
##  the Amulet hierarchy -- all other paths are computed from this.
##
##  Set your AMULET_VARS_FILE environment variable to the name of the file
##  that describes the compiler flags you want to use.  The value of this
##  variable is the name of a file in amulet/bin
##    Makefile.vars.<compiler>.<platform>
##
##  If an appropriate Makefile does not exist for your compiler/platform
##  customize Makefile.vars.custom

## Sets pathname variables and loads $(AMULET_VARS_FILE) to set
## compiler variables
include $(AMULET_DIR)/bin/Makefile.vars

## The default build has two parameters: OP and LIB_MODIFIER
## used to specify the variant of the Amulet library that will be used
## OP chooses one of the sets of compiler flags, defined in
## AMULET_VARS_FILE: AM_DEVELOP, AM_RELEASE, AM_INHOUSE, AM_NODEBUGSYM
OP=$(AM_DEVELOP)

## LIB_MODIFIER is appended to library filename to indicate particular
## OP choice: blank for AM_DEVELOP, -release for AM_RELEASE
##	      -inhouse for AM_INHOUSE 
LIB_MODIFIER=

## To link with the release library, invoke make as follows
##	Make OP='$(AM_RELEASE)' LIB_MODIFIER=-release

CFLAGS = $(OP) $(AM_CFLAGS)
AMULET_LIBRARY = amulet$(LIB_MODIFIER)

tutorial: tutorial.o
	$(CC) tutorial.o \
	-L$(LIB_DIR) -l$(AMULET_LIBRARY) $(AM_LIBS) \
	-o tutorial

tutorial.o: tutorial.cc
	$(CC) $(CFLAGS) -c tutorial.cc

clean:
	$(RM) tutorial *.o *~
