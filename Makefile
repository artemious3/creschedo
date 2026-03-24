.PHONY: all clean

all:
	$(MAKE) -C src creschedo

release:
	$(MAKE) -C src BUILD=release creschedo

clean:
	$(MAKE) -C src clean





