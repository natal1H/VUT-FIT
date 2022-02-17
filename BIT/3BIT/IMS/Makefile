CPP=g++
CXXFLAGS=-std=c++11
SOURCES=sir.cpp data.cpp main.cpp
HEADERS=sir.h data.h main.h
EXECUTABLE=ims_project

all: build

build: $(SOURCES) $(HEADERS)
	$(CPP) $(SOURCES) -o $(EXECUTABLE)

run: $(SOURCES) build
	./$(EXECUTABLE) $(ARGS)
	
exp1: $(SOURCES) build
	./$(EXECUTABLE) -f exp1.dat
	
exp2: $(SOURCES) build
	./$(EXECUTABLE) -m vysoke_skoly,stredne_skoly,2_stupen_ZS,skolky_1_stupen_ZS,internaty_zatvorene -f exp2.dat

exp3: $(SOURCES) build
	./$(EXECUTABLE) -m vysoke_skoly_polovica,stredne_skoly_polovica,2_stupen_ZS_polovica,skolky_1_stupen_ZS_polovica -f exp3.dat

exp4: $(SOURCES) build
	./$(EXECUTABLE) -m vysoke_skoly,internaty_zatvorene -f exp4.dat

exp5: $(SOURCES) build
	./$(EXECUTABLE) -m kina_zatvorene,divadla_zatvorene,muzea_galerie_zatvorene,hromadne_podujatia -f exp5.dat

exp6: $(SOURCES) build
	./$(EXECUTABLE) -m restauracie_kaviarne_zatvorene,obchody,bohosluzby_zakaz -f exp6.dat
	
exp7: $(SOURCES) build
	./$(EXECUTABLE) -m svadby_obrad,pohreby_rodina -f exp7.dat

exp8: $(SOURCES) build
	./$(EXECUTABLE) -m 2_stupen_ZS,stredne_skoly,vysoke_skoly,internaty_zatvorene,hromadne_podujatia_zakaz,kina_zatvorene,divadla_zatvorene,restauracie_kaviarne_6_osob -f exp8.dat
	
clean: $(EXECUTABLE)
	rm $(EXECUTABLE)
	
zip: $(SOURCES) $(HEADERS)
	zip 02_xholko02_xchrip01.zip Makefile dokumentacia.pdf $(SOURCES) $(HEADERS)
