all:
	gcc -c log.c -ggdb
	gcc -c servidor.c -ggdb
	gcc -c cliente.c -ggdb
	gcc -c dispatcher.c -ggdb
	gcc servidor.o log.o -o servidor -ggdb
	gcc cliente.o log.o -o cliente -ggdb
	gcc dispatcher.o log.o -o dispatcher -ggdb
	rm -rf *.o
	rm -rf *~
clean:
	rm -rf *.o
	rm -rf *~
copy:
	cp log.c log.c.txt
	cp log.h log.h.txt
	cp servidor.c servidor.c.txt
	cp cliente.c cliente.c.txt
	cp dispatcher.c dispatcher.c.txt
	cp log.c.txt /home/html/inf/gtk10/trabalhos/redes2/
	cp log.h.txt /home/html/inf/gtk10/trabalhos/redes2/
	cp servidor.c.txt /home/html/inf/gtk10/trabalhos/redes2/
	cp cliente.c.txt /home/html/inf/gtk10/trabalhos/redes2/
	cp dispatcher.c.txt /home/html/inf/gtk10/trabalhos/redes2/
