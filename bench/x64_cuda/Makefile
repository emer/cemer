# This Makefile just redirects to the one in the build subdirectory.
# It's only here to support the Debian MOTU scripts in the bin directory.

# Default target executed when no arguments are given to make.
default_target: all

distclean:
	rm -rf build

# Forward all make requests to build/Makefile
%:
	$(MAKE) -C build $@
