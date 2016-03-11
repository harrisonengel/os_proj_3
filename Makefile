# Makefile for Project 3
# Harrison Engel and Duy Dang
#

all: osproj3

osproj3: osproj3.cpp
	g++ -std=c++11 -o osproj3 osproj3.cpp -lpthread

clean:
	rm osproj3