PORT ?= /dev/ttyUSB0

default: original

original:
	@pio run -t upload -e $@ --upload-port $(PORT)
