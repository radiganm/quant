## Makefile: finance
## Mac Radigan

cwd = $(shell pwd)

.PHONEY: all octave-build jvm-build native-build

all: octave-build jvm-build native-build

octave-build:

jvm-build:
	(cd $(cwd)/$(@:-build=); mvn package)

native-build:
	(cd $(cwd)/$(@:-build=); cmake .)
	make -C $(cwd)/$(@:-build=)


## *EOF*
