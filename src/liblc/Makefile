# Filename: src/liblc/Makefile
# Project: liblc
# Brief: Build the liblc library
#
# Copyright (C) 2016 Guy Shaw
# Written by Guy Shaw <gshaw@acm.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

LIBRARY := liblc
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

CC := gcc
### CONFIG := -DDEBUG -D_POSIX_C_SOURCE=200809
CONFIG := -DDEBUG
CPPFLAGS := -I../inc
CFLAGS += -std=c99 -g -Wall -Wextra $(CONFIG)

.PHONY: all clean show-targets

all: $(LIBRARY).a

$(LIBRARY).a: $(OBJECTS)
	ar crv $(LIBRARY).a $(OBJECTS)

clean:
	rm -f $(LIBRARY).a $(OBJECTS) *.o
	cscope-clean

show-targets:
	@show-makefile-targets

show-%:
	@echo $*=$($*)
