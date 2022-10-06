CC = g++ -std=c++17
CFLAGS = -Wall -Iinclude -g

### libsync

HEADER = $(wildcard ./include/*.h)
SRC = $(wildcard ./src/*.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))
DEP = ${OBJ:.o=.d}

sync : $(HEADER) $(OBJ)
	$(CC) $(CFLAGS) -lpthread $(OBJ) -o sync

-include ${DEP}

%.o : %.cc
	$(CC) $(CFLAGS) -MMD -c $< -o $@

### integration test

ITESTS = $(wildcard ./itest/*.py)

.PHONY: itest $(ITESTS)

itest : sync $(ITESTS)

$(ITESTS): % : sync
	@echo "\n  Testing $@\n========================\n"
	@python $@
	@echo "\n========================\n"

### unit test

.PHONY: clean
clean :
	rm -f src/*.o src/*.d
	rm -f sync