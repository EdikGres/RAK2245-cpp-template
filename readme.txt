for raspberry compilation need change ARCH and CROSS_COMPILE in Makefile.

x86:
ARCH ?= 
CROSS_COMPILE ?=

arm:
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-
