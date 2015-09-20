TARGET        := ping
SOURCE        := ping.cpp

CXX           := g++
CXXFLAGS      := -std=c++11
#CXXFLAGS      := -g -W -O3 -pedantic -Wall -ansi -std=c++11

$(TARGET): clean
	@echo " $(CXX) -o $(TARGET) $(SOURCE) $(CXXFLAGS)"; $(CXX) -o $(TARGET) $(SOURCE) $(CXXFLAGS)
	@echo " Usage: ./ping <address> <port>"

clean:
	@echo " rm -f ./ping"; rm -f ./ping
