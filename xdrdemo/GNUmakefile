
SUBDIRS = cxx

all:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done

clean:
	rm -f *~ .*~
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done

.PHONY: all clean
