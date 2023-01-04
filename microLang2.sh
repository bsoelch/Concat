#!/bin/sh

gcc -g -Wall -Wextra -Wshadow -Wold-style-definition -Wcast-qual ./microLang2.c -o "microLang2" && ./microLang2 "./code.txt"
