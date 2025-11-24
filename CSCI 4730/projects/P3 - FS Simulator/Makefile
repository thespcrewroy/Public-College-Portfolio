all: fs tests

fs: fs_sim.c fs.c fs.h fs_util.c disk.c disk.h
		gcc fs_sim.c fs.c disk.c fs_util.c -g -o fs_sim

# Build tests
.tests: tests.c fs.c fs.h fs_util.c disk.c disk.h
		gcc tests.c fs.c disk.c fs_util.c -g -o tests

.PHONY: tests

# Run tests target builds and executes tests
tests: .tests
		./tests

clean:
		rm -f fs_sim tests

