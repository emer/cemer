# debugging makefile, just redirects to build_dbg subdir -- for compiling within
# emacs or similar within ta directory

default_target: all

# Forward all make requests to build_dbg/Makefile
%:
	$(MAKE) -C ../../../build_dbg $@
