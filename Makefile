.PHONY: all clean

all:
	$(MAKE) -C src creschedo

release:
	$(MAKE) -C src BUILD=release rbobin

clean:
	$(MAKE) -C src clean





