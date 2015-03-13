###########################################
#Makefile for simple programs
#export LD_LIBRARY_PATH=/home/zwd/codes/libs/boost/lib/
#g++ -I
###########################################

.PHONY:clean
.DEFAULT: pdfs

CXX := clang
INC := -I.#/third_party/boost/include/
LDFLAGS := -L./thirdparty/lib/ 
LDLIBS := -lstdc++ -lm -ludt#-lboost_system -lboost_filesystem -lboost_serialization -lboost_thread
CXXFLAGS := -Wall -g -std=c++11 -pthread $(INC)
OBJS := ThreadPool.o \
		LocalFileSystem.o \
		UDPNetwork.o \
		TCPNetwork.o \
		DuplicationManager.o \
		NetworkInterface.o \
		UDPNetworkInterface.o \
		TCPNetworkInterface.o \
		UDPNetworkHandler.o \
		TCPNetworkHandler.o \
		SClusterHandler.o \
		ClusterHandler.o \
		BroadcastHandler.o \
		Redistribution.o \
		Cluster.o \
		Benchmark.o

all: pdfs

pdfs: $(OBJS)
	$(CXX) $(CXXFLAGS) $@.cpp -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

pdfsc: $(OBJS)
	$(CXX) $(CXXFLAGS) $@.cpp -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

push: $(OBJS)
	clang -g -std=c++11 -lstdc++ -lpthread $@.cpp -o $@ $^ $(LDLIBS)
pull: $(OBJS) 
	clang -g -std=c++11 -lstdc++ -lpthread $@.cpp -o $@ $^ $(LDLIBS)

benchmark: $(OBJS)
	clang -g -std=c++11 -lstdc++ -lpthread $@.cpp -o $@ $(OBJS) $(LDLIBS)

# FIXME: dependency info is not enough
%.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $< -o $@ 

# The following region is for clear usage
clean:
	@echo "Removing linked and compiled files......"
	rm -f $(OBJS) pdfs pdfsc push pull benchmark
