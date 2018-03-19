CC=gcc
CXX=g++

CFLAGS=-Wall -g -O0
CPPFLAGS=
CXXFLAGS=-Wall -g -O0 -std=c++11

LDFLAGS=-lncurses \

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
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) $< $(LDFLAGS) -o $@

$(CXXPROGS): %: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) $< $(LDFLAGS) -o $@

prog: $(OBJS) $(PROGS) $(CXXPROGS)

