PROTOCOL ?= usbasp
PORT ?= usb

default: original

original:
	@pio run -t upload -e $@ --upload-protocol $(PROTOCOL) --upload-port $(PORT)
