#!/bin/sh

gcc -g -Wall -Wextra -Wshadow -Wold-style-definition -Wcast-qual ./compile.c && ./a.out "./in.txt"
