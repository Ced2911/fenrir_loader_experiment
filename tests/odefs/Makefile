target= odefs-test

all:
	gcc -g -fprofile-arcs -ftest-coverage -std=c2x -D_DEFAULT_SOURCE -DUNIT_TEST odefs.c test.c   -o $(target) 

clean:
	rm -f $(OBJS) $(OUT)