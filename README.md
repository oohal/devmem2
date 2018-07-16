# devmem2

Original: http://www.lartmaker.nl/lartware/port/devmem2.c

This version fixes a few bugs, namely:
	- Actually handles page sizes other than 4KB.
	- Adds support for 64bit accesses
	- Forces the access sizes for b/h/w/l to 8/16/32/64 bits respectively.

# Building

```
make devmem2
```

Yeah that works. Suprised me too.
