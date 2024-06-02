CC = gcc
CFLAGS = -Wall -g
DEPS = main.h
OBJ = main.o scheduler.o gantt_chart.o evaluation.o process.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

scheduler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	rm -f *.o

.PHONY: clean

clean:
	rm -f *.o scheduler
