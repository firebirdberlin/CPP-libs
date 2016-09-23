
CC=colorgcc

all: dat2IGB IGB2dat IGBdiff

dat2IGB: dat2IGB.cpp
	$(CC) $(OPT) dat2IGB.cpp `mylibs-config --cldflags` `myImg-config --cflags` `myImg-config --ldflags` -o dat2IGB.`uname -m`
	cd $(MYLOCAL)/bin && ln -sf  $(PWD)/dat2IGB.`uname -m` dat2IGB.`uname -m`

IGB2dat: IGB2dat.cpp
	$(CC) $(OPT) IGB2dat.cpp `mylibs-config --cldflags` `myImg-config --cflags` `myImg-config --ldflags` -o IGB2dat.`uname -m`

IGBdiff: IGBdiff.cpp
	$(CC) $(OPT) IGBdiff.cpp -fopenmp `mylibs-config --cldflags` `myImg-config --cflags` `myImg-config --ldflags` -o IGBdiff.`uname -m`
	cd $(MYLOCAL)/bin && ln -sf  $(PWD)/IGBdiff.`uname -m` IGBdiff.`uname -m`


clean:
	rm -rf *.o dat2IGB.`uname -m`
