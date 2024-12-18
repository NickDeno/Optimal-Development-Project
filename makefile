all: submission

submission: submission.o
	g++ -o submission submission.o

submission.o: submission.cpp
	g++ -c -g submission.cpp

clean:
	rm -f *.o submission

# all: myConvex

# myConvex: myConvex.o
# 	g++ -o myConvex myConvex.o

# myConvex.o: myConvex.cpp
# 	g++ -c -g myConvex.cpp

# clean:
# 	rm -f *.o myConvex