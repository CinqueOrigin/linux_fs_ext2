o _file  = init.o main.o
target : $(o_file)
	gcc -o target $(o_file)
init.o : init.o
	gcc -c init.c
main.o : main.o
	gcc-c main.c
clean :
	rm $(o_file) target