CC=gcc
CXX=g++

CFLAGS=-Wall -g -O0

CPPFLAGS=-pthread

CXXFLAGS=-Wall -g -O0 -std=c++11

OBJS=

PROGS=	client \

CXXPROGS=
all: $(OBJS) prog

clean:
	rm -f $(OBJS)
	rm -f $(PROGS)
	rm -f $(CXXPROGS)
	rm -f *.o
	rm -f *~

$(PROGS): %: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) $< -o $@

$(CXXPROGS): %: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) $< -o $@

prog: $(OBJS) $(PROGS) $(CXXPROGS)

