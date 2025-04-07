CC = gcc
CFLAGS =  
DEBUG = debug

all: target target/$(DEBUG)

# Run target
run: target target/$(DEBUG)
	./target/$(DEBUG)

# Unity build
target/$(DEBUG): src/main.c
	$(CC) $^ -o $@ $(CFLAGS)

# directory check
target:
	@echo "Creating directory ./target"
	mkdir -p ./target


# Clean target
clean:
	rm -rf obj target/$(DEBUG)
