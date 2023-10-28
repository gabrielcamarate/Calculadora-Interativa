calculadora:
#	Compilando o arquivo [-c] | [-o] output (gera um objeto intermediario "pré-compliado")
	gcc -c calculadora.c -o calculadora.o

#	Lincando para .exe
	gcc -o calculadora calculadora.o `pkg-config --libs ncursesw`
	rm *.o