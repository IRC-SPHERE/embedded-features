ARCHITECTURE ?= native

#V=1

ifeq ($(ARCHITECTURE),native)

##############################################
# For native (x86) executable
##############################################

EXE = feature-test

CFLAGS += -O2 -g
LDFLAGS += -lm

all:
	gcc $(CFLAGS) main.c -o $(EXE) $(LDFLAGS)

clean:
	rm -f $(EXE)

run: all
	./$(EXE)

else
ifeq ($(ARCHITECTURE),sphere)

##############################################
# For SPHERE boards
##############################################

BOARD=spes-2/cc2650

#TARGET_LIBFILES+= -L/home/atis/sources/CMSIS/CMSIS/Lib/GCC -larm_cortexM3l_math
#CFLAGS+= -I/home/atis/sources/CMSIS/CMSIS/Include
#CFLAGS+= -DARM_MATH_CM3

CFLAGS += -DCC26XX_UART_CONF_BAUD_RATE=1000000

CONTIKI_PROJECT = main
all: $(CONTIKI_PROJECT)

# set a fake address - not used anyway, but required by the upload script
A=1

run: $(CONTIKI_PROJECT).upload

CONTIKI = /home/atis/sphere/deployment/sphere-contiki
include $(CONTIKI)/Makefile.include

else
ifeq ($(ARCHITECTURE),z1)

##############################################
# For Zolertia Z1
##############################################

TARGET=z1

CONTIKI_PROJECT = main
all: $(CONTIKI_PROJECT)

run: $(CONTIKI_PROJECT).upload

CONTIKI = /home/atis/work/contiki-mainline-git
include $(CONTIKI)/Makefile.include

else

ifeq ($(ARCHITECTURE),zoul)

##############################################
# For Zolertia Zoul
##############################################

TARGET=zoul

CONTIKI_PROJECT = main
all: $(CONTIKI_PROJECT)

run: $(CONTIKI_PROJECT).upload

CONTIKI = /home/atis/work/contiki-mainline-git
include $(CONTIKI)/Makefile.include

else

ifeq ($(ARCHITECTURE),nrf52dk)

##############################################
# For Nordic nrf52dk
##############################################

TARGET=nrf52dk

CONTIKI_PROJECT = main
all: $(CONTIKI_PROJECT)

run:
	make erase && make softdevice.flash && make $(CONTIKI_PROJECT).flash

CONTIKI = /home/atis/work/contiki-mainline-git
include $(CONTIKI)/Makefile.include

else

##############################################
# For Zephyr
##############################################

obj-y = main.o

endif
endif
endif
endif
endif
