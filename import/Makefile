#CFLAGS = -std=gnu++11 -Wall -O2 #-std=c++11 -Wextra -W -O3 -g
#CFLAGS = -Wall -O2 -std=c++11 #-Wextra -W -O3 -g

# the `O2` compile flag is selected as it is used for building CC2650 in Contiki
OPTIMIZATION = -O2

CFLAGS = $(OPTIMIZATION) -std=c11 -g -W -Wall -Wextra

#	gcc a.c -E -std=c11 -W -g -O3 -o a.e

SRC ?= main.c

EXE ?= feature_extract

all:
	$(CC) $(CFLAGS) $(SRC) -o $(EXE) $(LDFLAGS)

clean:
	rm -f $(EXE)
