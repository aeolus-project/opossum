################################
##DIRTY MAKEFILE WHILE WAITING FOR CMAKE
################################
CXXFLAGS =	-ansi -pedantic -g -Wall -fmessage-length=0

OBJS =		src/network.o src/test-generate.o 

LIBS =

TARGET =	opposum

$(TARGET):	$(OBJS)
	$(CXX)  -o $(TARGET) $(OBJS) $(LIBS) 

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
