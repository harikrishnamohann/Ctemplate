CC = clang
CFLAGS = -std=c23 -Wall -Werror
DEBUG = debug
RELEASE = release

all: target target/$(DEBUG)

release: target/$(RELEASE)

# Run target
run: target target/$(DEBUG)
	./target/$(DEBUG)


target/$(RELEASE): src/* lib/*
	$(CC) -o3 src/main.c -o $@

target/$(DEBUG): src/* lib/*
	$(CC) -g $(CFLAGS) src/main.c -o $@ 

# directory check
target:
	@echo "Creating directory ./target"
	mkdir -p ./target

