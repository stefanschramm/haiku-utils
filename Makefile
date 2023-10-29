all: makebootabletiny

makebootabletiny: makebootabletiny.c
	gcc "$<" -o "$@"

clean:
	rm -f makebootabletiny
