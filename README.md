# CTemplate  
> _~ A minimal template for simple C projects_

**CTemplate** is a lightweight project structure designed to help organize small-to-medium C programs. It encourages the use of a **unity build system** and includes a few personal utility libraries:

- A simple `String` type implementation  
- An `Arena` allocator  
- Basic `error handling` mechanism

This isn't an all-in-one C framework, but rather a personal toolkit that grows as needed.

---

## 📁 Project Structure

```
Ctemplate/
├── lib/               # Home for second-level APIs (libraries)
│   ├── arena.c        # Arena memory allocator
│   ├── err.c          # Error handling macros
│   ├── strings.c      # String type and manipulation functions
│   └── utils.c        # utility functions
├── build.sh           # Compiles the project into the /target directory
├── README.md          # This file
├── src/               # Your project files go here
│   └── main.c
└── target/            # Compiled binaries live here
    └── debug
```

---

## 🚀 Getting Started

To use this template for your project:

```bash
git clone https://github.com/harikrishnamohann/Ctemplate.git
mv Ctemplate <your-project-name>
```

Then, remove the Git history and this README.md file:

```bash
rm -rf <your-project-name>/.git
rm <your-project-name>/README.md
```

You're now ready to start coding!

---

## 🧠 How to Use the Libraries

To use any library from `lib/`, just include it in your `main.c` (or any other C file):

```c
#include "../lib/strings.c"

int main() {
    String s = str_init("hello world");
    str_debug_print(s);
    str_free(&s);
    return 0;
}
```

---

## 📌 Notes

- I am using a **unity build system** for managing multiple files.
- External libraries can be specified in LIBS array in build.sh as it'll ensure the existance of those libraries before compiling.

#### Compiling:
```bash
./build.sh debug
# or
./build.sh release
```

#### compiling and running :
```bash
./build.sh debug run
# or
./build.sh release run
# or
./target/debug
```

---

## 💬 Final Words

Thanks for checking out my silly stuff!  
Hope you find it helpful or fun to explore. `^-^`
