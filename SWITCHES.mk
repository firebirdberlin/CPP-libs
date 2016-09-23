
HAVE_GSL = $(shell which gsl-config)
ifneq ($(HAVE_GSL), )
	LDFLAGS += `gsl-config --libs`
	CFLAGS  += `gsl-config --cflags`
	DFLAGS  += -DHAVE_GSL
endif
