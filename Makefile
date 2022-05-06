PORT ?= usb

default: original

original:
	@pio run -t upload -e $@ --upload-port $(PORT)

upload:
	@avrdude -p m328pb -c usbasp -U flash:w:.pio/build/original/firmware.hex:i
