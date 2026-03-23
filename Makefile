.PHONY: all clean

all:
	$(MAKE) -C src rbobin

release:
	$(MAKE) -C src BUILD=release rbobin

clean:
	$(MAKE) -C src clean





