# =====================================================================
# author: Jean-Michel RICHER
# email: jean-michel.richer@univ-angers.fr
# date: January 2023
# =====================================================================

PROJECT_NAME=sudoku
PROJECT_VERSION=1.0
PROJECT_DIR=$(shell pwd)

TODAY=$(shell date '+%Y_%m_%d_%Hh_%M')
TAR_ARCHIVE_NAME="$(HOME)/export/$(PROJECT_NAME)_$(PROJECT_VERSION)_$(TODAY).tgz"
ZIP_ARCHIVE_NAME="$(HOME)/export/$(PROJECT_NAME).zip"

BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj
LIB_DIR=$(BUILD_DIR)/lib
BIN_DIR=$(BUILD_DIR)/bin
SRC_DIR=src
LIBRARY=$(LIB_DIR)/sudoku_lib.a

CPP_FLAGS= -Wall -std=c++11 -ggdb -O3 -fopenmp

CUDA_ARCH= --optimize 3 \
	-gencode arch=compute_86,code=sm_86 \
	-gencode arch=compute_75,code=sm_75 \
	-gencode arch=compute_62,code=sm_62 \
	-gencode arch=compute_60,code=sm_60 
			
CUDA_FLAGS=-O3

.SUFFIXES: .o .cpp .cu
.PHONY: tests

all: create_directories \
	 create_library \
	 $(BIN_DIR)/sudoku_cpu_recursive.exe \
 	 $(BIN_DIR)/sudoku_cpu_iterative.exe \
 	 $(BIN_DIR)/sudoku_cpu_recursive_parallele.exe \
 	 $(BIN_DIR)/sudoku_cpu_iterative_parallele.exe  \
 	 $(BIN_DIR)/sudoku_gpu_iterative_parallele.exe  

create_directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

create_library: $(LIBRARY)

$(LIBRARY): $(OBJ_DIR)/position.o $(OBJ_DIR)/grid.o $(OBJ_DIR)/block_cost.o
	@echo "- generate library $(LIBRARY)"
	@ar rv $@ $^ >/dev/null 2>&1
	@ranlib $@

	
$(BIN_DIR)/sudoku_cpu_recursive.exe: $(OBJ_DIR)/sudoku_cpu_recursive.o  $(LIBRARY) 
	g++ -o $@ $^ $(CPP_FLAGS)

$(BIN_DIR)/sudoku_cpu_iterative.exe: $(OBJ_DIR)/sudoku_cpu_iterative.o $(LIBRARY)
	g++ -o $@ $^ $(CPP_FLAGS)

$(BIN_DIR)/sudoku_cpu_recursive_parallele.exe: $(OBJ_DIR)/sudoku_cpu_recursive_parallele.o $(LIBRARY)
	g++ -o $@ $^ $(CPP_FLAGS) 

$(BIN_DIR)/sudoku_cpu_iterative_parallele.exe: $(OBJ_DIR)/sudoku_cpu_iterative_parallele.o $(LIBRARY)
	g++ -o $@ $^ $(CPP_FLAGS)

$(OBJ_DIR)/gpu_grid.o: src/gpu_grid.cu
	nvcc --compile -o $@ $< --compiler-options -O3 $(CUDA_ARCH) $(CUDA_FLAGS) 

$(BIN_DIR)/sudoku_gpu_iterative_parallele.exe: $(OBJ_DIR)/sudoku_gpu_iterative_parallele.o $(LIBRARY) 
	@echo "LINK "
	nvcc --link -o $@ $^ $(CUDA_FLAGS) $(CUDA_ARCH)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cu
	nvcc --compile -o $@ $< --compiler-options -O3 $(CUDA_ARCH) $(CUDA_FLAGS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ -c -o $@ $< $(CPP_FLAGS)

clean:
	rm -rf $(BIN_DIR)/*.exe $(OBJ_DIR)/*.o $(LIB_DIR)/*.a

tests:
	@chmod +x ./tests.sh
	./tests.sh

archive:
	@echo "- generate archive " ;\
	mkdir -p ~/export ;\
	date +'%Y/%m/%d %Hh%M' >timestamp ;\
	echo "file:\n$(TAR_ARCHIVE_NAME)" ;\
	mkdir -p `dirname $(TAR_ARCHIVE_NAME)` ;\
	cd .. ; \
	tar -czf $(TAR_ARCHIVE_NAME) $(PROJECT_NAME) ; \
	zip -r $(ZIP_ARCHIVE_NAME) $(PROJECT_NAME) ; \
	echo "--------------------------------" ;\
	echo "Archive send to:" ; \
	echo "$(TAR_ARCHIVE_NAME)" ;\
	echo "$(ZIP_ARCHIVE_NAME)" ;\
	cd $(PROJECT_DIR) 	


