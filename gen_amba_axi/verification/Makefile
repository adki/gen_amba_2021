DIRS	= $(subst /,, $(dir $(wildcard */Makefile)))

all:

clean cleanup clobber cleanupall:
	/bin/rm -rf design
	for D in $(DIRS); do\
		if [ -f $$D/Makefile ] ; then \
			echo "make -C $$D -s $@";\
			make -C $$D -s $@;\
		fi;\
	done
	@if [ -f ip/test_axi4to3to4/Makefile ] ; then \
		make -C ip/test_axi4to3to4 $@;\
	fi

.PHONY: all clean cleanup clobber cleanupall
