.PHONY: all clean

all:
	bear -- $(MAKE) -C src creschedo

release:
	bear -- $(MAKE) -C src BUILD_TYPE=release creschedo

clean:
	$(MAKE) -C src clean
