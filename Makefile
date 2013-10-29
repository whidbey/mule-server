GEN_SRC := muled.cpp gen-cpp/interface_constants.cpp gen-cpp/interface_constants.cpp  gen-cpp/Interface.cpp gen-cpp/interface_types.cpp inih/ini.c inih/cpp/INIReader.cpp
GEN_OBJ := $(patsubst %.cpp,%.o, $(GEN_SRC))
DEP_HEAD := CustomHandler.h

THRIFT_DIR := /usr/local/include/thrift

INC := -I$(THRIFT_DIR) -Igen-cpp/ -Iinclude/ -Iinih/ -Iinih/cpp/ -Iconnection-pool/
LIB := -L/usr/local/lib 

all: .thriftgen muled

.thriftgen:
	thrift --gen cpp interface.thrift
	thrift --gen perl interface.thrift
	touch .thriftgen

%.o: %.cpp
	$(CXX) -Wall -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H $(INC) -c $< -o $@

muled: muled.o $(GEN_OBJ)
	$(CXX) $^ -o $@ $(LIB) -lboost_thread -lthrift -lthriftnb -levent -lmysqlcppconn

clean:
	$(RM) *.o gen-cpp/*.o inih/*.o inih/cpp/*.o muled .thriftgen