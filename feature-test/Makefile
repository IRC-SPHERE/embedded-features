#ARCHITECTURE ?= native
#ARCHITECTURE ?= zephyr
ARCHITECTURE ?= sphere

ifneq ("$(WINDOW_SIZE)","")
CFLAGS += -DTIME_WINDOW_SIZE=$(WINDOW_SIZE)
CFLAGS += -DFREQUENCY_FEATURE_WINDOW_SIZE=$(WINDOW_SIZE)
endif

ifneq ("$(FEATURE_TEST_INPUT)","")
CFLAGS += -DFEATURE_TEST_INPUT=\\\"$(FEATURE_TEST_INPUT)\\\"
endif

ifneq ("$(DO_LOG_OUTPUT)","")
CFLAGS += -DDO_LOG_OUTPUT=$(DO_LOG_OUTPUT)
endif

ifeq ("$(ARCHITECTURE)","zephyr")

BOARD ?= "qemu_cortex_m3"

CONF_FILE = prj.conf
include ${ZEPHYR_BASE}/Makefile.test

else

all:
	make -C src all ARCHITECTURE=$(ARCHITECTURE) CFLAGS="$(CFLAGS)"

clean:
	make -C src clean ARCHITECTURE=$(ARCHITECTURE) CFLAGS="$(CFLAGS)"

run:
	make -C src run ARCHITECTURE=$(ARCHITECTURE) CFLAGS="$(CFLAGS)"

endif
