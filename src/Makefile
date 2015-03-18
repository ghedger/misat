IDIR =../include
CC=g++
CFLAGS=-std=c++11 -I$(IDIR) -g

ODIR=../obj
BINDIR=../bin
LDIR =../lib

LIBS=-lm

_DEPS = iffdigest.h audiofile.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = FFT.o misat.o iffdigest.o RealFFTf48x.o RealFFTf.o audiofile.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/misat: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
