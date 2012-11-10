# 
# Author: Arun Tejasvi Chaganty <arunchaganty@gmail.com>
# Date:   2010-02-04
#

ROOTDIR  = ../
include $(ROOTDIR)Makefile.inc

# Add your bot(s) here
BOTS   	 = RandomBot TuringBot SlowBot GBot TestBot-v1.0 d2 OrderedAB
DIRS	 = $(BOTS)

all: ${BOTS}

${BOTS}: % : force_look
	echo Looking into subdir $@ 
	cd $@; make

.PHONY: clean

clean:
	-for d in $(DIRS); do (cd $$d; make clean ); done

force_look:
	true

