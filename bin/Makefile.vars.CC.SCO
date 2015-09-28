##
##  This file sets the Amulet Makefile variables for:
##	PC's running SCO Unixware 2.1 (Personal Edition)
##	with the SCO Unixware SDK 2.1 C++ compiler
##  On Pentium PC's use compiler flag -Kpentium
##  On 486 PC's, if that doesn't work, use -Kblended
##
##  This Makefile.vars file was contributed by:
##	Arto V. Viitanen <av@cs.uta.fi>
##	University of Tampere, Department of Computer Science
##	Tampere, Finland
##  and updated for Amulet V3 by the Amulet group.
##    
##  The Amulet group has not tested it.  Please send any bug reports to
##  amulet-bugs@cs.cmu.edu.

CC = /usr/ccs/bin/CC
LD = ld

##
## Parameters for Amulet programs
## 

# compiler flags for Amulet programs
AM_CFLAGS = -I$(AMULET_DIR)/include -I/usr/local/include \
	    -L/usr/include/CC -DCC -DNEED_BOOL -DMEMORY \
	    -DNEED_STRING

# libraries needed by Amulet (interactors needs math library)
AM_LIBS = -lX11 -lnsl -ldl -lm 


##
## Parameters for the Amulet library
## 

# directory where Amulet will (eventually) be installed
# defaults to its current location (AMULET_DIR)
AMULET_ROOT=$(AMULET_DIR)

# additional compiler flags for development version
AM_DEVELOP = -Kpentium -O -DDEBUG +d -g

# additional compiler flags for release version
AM_RELEASE = -Kpentium -O 

# additional compiler flags for optimized version with inspector and debugging enabled
AM_INHOUSE = -DDEBUG +d -g

# additional compiler flags for optimized version with inspector but no debugging symbols
AM_NODEBUGSYM = -Kpentium -O -DDEBUG

# extra link flags for library test programs
AM_LIB_LDFLAGS =

# Makefile identifying the gem files for this platform
AM_GEM_SOURCE = Makefile.gem.X

# Makefile that knows how to make the Amulet library for this platform
AM_STATIC_LIB_MAKE = Makefile.lib.unix.static

# Set of library variants for make libs
AM_LIBS_TO_MAKE = develop-static release-static
