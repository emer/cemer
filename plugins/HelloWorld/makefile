INCLUDES = -I/home/mingus/pdp4.0/trunk/src/ta/../../. -I/home/mingus/pdp4.0/trunk/src/ta -I/home/mingus/pdp4.0/trunk/src/ta/../ta/ios-g++-3.1 -I/home/mingus/pdp4.0/trunk/src/ta/../pdp -I/home/mingus/pdp4.0/trunk/src/ta/../ta -I/home/mingus/pdp4.0/trunk/src/ta/../css -I/home/mingus/pdp4.0/trunk/src/ta/../leabra -I/home/mingus/pdp4.0/trunk/src/ta/../tamisc -I/home/mingus/pdp4.0/trunk/src/ta/../taiqtso -I/home/mingus/pdp4.0/trunk/src/ta/../bp -I/home/mingus/pdp4.0/trunk/src/ta/../tamisc

all: clean qmake
	${MAKE} -f Makefile

clean:
	rm -f *~ \#* *TA* moc_hw.cpp Makefile

ta:
	/home/mingus/bin/maketa -css -cpp="g++ -E" $(INCLUDES) hw hw.h

moc:
	/usr/local/Trolltech/Qt-4.1.0/bin/moc $(INCLUDES) -o moc_hw.cpp hw.h

qmake: ta
	${QTDIR}/bin/qmake -o Makefile hw.pro
