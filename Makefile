# Makefile for Project 3
# Harrison Engel and Duy Dang
#

all: proj3

proj3: proj3.cpp
	g++ -o proj3 proj3.cpp -lpthread
