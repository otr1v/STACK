all:
	g++ main.cpp func.cpp -o stack.out 
debug:
	g++ -g main.cpp func.cpp -o stack.out
clean:
	rm -f stack.out