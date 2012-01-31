##############################################################
#               CMake Project Wrapper Makefile               #
############################################################## 

SHELL := /bin/bash
RM    := rm -rf

install: all

all:
	@- (cd build >/dev/null 2>&1 && cmake -D CMAKE_BUILD_TYPE:STRING=RELEASE -D BUILD_TESTS:BOOL=TRUE START_TESTS:BOOL=TRUE ..)
	@- $(MAKE) -C build

release:
	@- (cd build >/dev/null 2>&1 && cmake -D CMAKE_BUILD_TYPE:STRING=RELEASE -D BUILD_TESTS:BOOL=FALSE START_TESTS:BOOL=FALSE ..)
	@- $(MAKE) -C build

debug:
	@- (cd build >/dev/null 2>&1 && cmake -D CMAKE_BUILD_TYPE:STRING=DEBUG -D BUILD_TESTS:BOOL=FALSE START_TESTS:BOOL=FALSE ..)
	@- $(MAKE) -C build

test: all

cleanrelease: clean
	@- $(RM) -R ./bin/Release/*

cleandebug: clean
	@- $(RM) -R ./bin/Debug/*

cleantest: clean
	@- $(RM) -R ./bin/Test/*

clean:
	@- (cd build >/dev/null 2>&1 && cmake .. >/dev/null 2>&1)
	@- $(MAKE) --silent -C build clean || true
	@- $(RM) ./build/Makefile
	@- $(RM) ./build/src
	@- $(RM) ./build/test
	@- $(RM) ./build/CMake*
	@- $(RM) ./build/cmake.*
	@- $(RM) ./build/*.cmake
	@- $(RM) ./build/*.txt
	@- $(RM) ./docs/*.html
	@- $(RM) ./docs/*.css
	@- $(RM) ./docs/*.png
	@- $(RM) ./docs/*.jpg
	@- $(RM) ./docs/*.gif
	@- $(RM) ./docs/*.tiff
	@- $(RM) ./docs/*.php
	@- $(RM) ./docs/search
	@- $(RM) ./docs/installdox
	
cleanall: distclean

distclean:
	@- (cd build >/dev/null 2>&1 && cmake .. >/dev/null 2>&1)
	@- $(MAKE) --silent -C build clean || true
	@- $(RM) ./build/Makefile
	@- $(RM) ./build/src
	@- $(RM) ./build/test
	@- $(RM) ./build/CMake*
	@- $(RM) ./build/cmake.*
	@- $(RM) ./build/*.cmake
	@- $(RM) ./build/*.txt
	@- $(RM) ./docs/*.html
	@- $(RM) ./docs/*.css
	@- $(RM) ./docs/*.png
	@- $(RM) ./docs/*.jpg
	@- $(RM) ./docs/*.gif
	@- $(RM) ./docs/*.tiff
	@- $(RM) ./docs/*.php
	@- $(RM) ./docs/search
	@- $(RM) ./docs/installdox
	@- $(RM) -R ./bin/*


#ifeq ($(findstring distclean,$(MAKECMDGOALS)),)
#    $(MAKECMDGOALS): ./build/Makefile
#	@ $(MAKE) -C build $(MAKECMDGOALS)
#endif
