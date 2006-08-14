INCLUDES = -I${PDP4DIR}/src/ta/../../. -I${PDP4DIR}/src/ta -I${PDP4DIR}/src/ta/../ta/ios-g++-3.1 -I${PDP4DIR}/src/ta/../pdp -I${PDP4DIR}/src/ta/../ta -I${PDP4DIR}/src/ta/../css -I${PDP4DIR}/src/ta/../leabra -I${PDP4DIR}/src/ta/../tamisc -I${PDP4DIR}/src/ta/../taiqtso -I${PDP4DIR}/src/ta/../bp -I${PDP4DIR}/src/ta/../tamisc

all: clean qmake
	${MAKE} -f Makefile

clean:
	rm -f *~ \#* *TA* moc_hw.cpp Makefile

ta:
	~/bin/maketa -D__MAKETA__ -css -cpp="g++ -E" $(INCLUDES) hw hw.h

moc:
	${QTDIR}/bin/moc $(INCLUDES) -o moc_hw.cpp hw.h

qmake: ta
	${QTDIR}/bin/qmake -o Makefile hw.pro
