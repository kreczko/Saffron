CC=g++
CFLAGS=-c -Wall `root-config --cflags --libs` --std=c++0x -O3
ODIR=obj
#LDFLAGS=
SOURCES=Saffron.cpp src/SafRunner.cpp src/SafDataSet.cpp src/SafGeometry.cpp \
        src/SafRawDataChannel.cpp src/SafRawDataSet.cpp src/SafAlgorithm.cpp \
        src/SafEventBuilder.cpp src/SafRawPlots.cpp src/SafTrigger.cpp \
        src/SafTriggerPlots.cpp src/SafPeakFitter.cpp src/SafFilter.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=saffron

all: $(SOURCES) $(EXECUTABLE) 
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ `root-config --cflags --libs`

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(SOURCES:.cpp=.o)
