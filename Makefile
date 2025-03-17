# Use `make run` to run this project on Abstract Machine with qemu
smp				:= 4
NAME			:= kernel
SRCS			:= framework/main.c $(shell find -L ./src/ -name "*.c")
INC_PATH		:= include/ framework/
export ARCH := x86-qemu
export AM_HOME := $(PWD)/abstract-machine
export CROSS_COMPILE := x86_64-linux-musl-  # It may need to be modified
include $(AM_HOME)/Makefile

log:
	make -nB \
	     | grep -ve '^\(\#\|echo\|mkdir\|make\)' \
	     | sed "s#$(AM_HOME)#\AM#g" \
	     | sed "s#$(PWD)#.#g" \
	     > compile.log
