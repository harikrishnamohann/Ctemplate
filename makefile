CC = gcc
CFLAGS =  
DEBUG = debug

all: obj target target/$(DEBUG)

# Run target
run: obj target target/$(DEBUG)
	./target/$(DEBUG)

target/$(DEBUG): obj/main.o obj/strings.o
	$(CC) $^ -o $@ $(CFLAGS)

# Object files
obj/main.o: src/main.c
	$(CC) -c $< -o $@

obj/strings.o: lib/strings.c
	$(CC) -c $< -o $@

# directory check
obj:
	@echo "Creating directory ./obj"
	mkdir -p ./obj

target:
	@echo "Creating directory ./target"
	mkdir -p ./target


# Clean target
clean:
	rm -rf obj target/$(DEBUG)
