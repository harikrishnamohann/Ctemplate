# CTemplate
> _~ A template for simple C projects_

This is a simple directory structure that I use for my C projects. It encourages the use of a unity build system. It offers an implementation of String type, An arena allocator and some error handling mechanisms that I use personally. I am adding new things to the structure when a need arise. So this is not a template that offers api's for all sort of things.

Here is the directory structure or template structure
```
Ctemplate/
├── lib :::::::::::::::: home for all second level api's
│   ├── arena.c
│   ├── err.c
│   └── strings.c
├── makefile ::::::::::: for compiling to target/
├── README.md
├── src :::::::::::::::: contains project files
│   └── main.c
└── target ::::::::::::: executable lives here
    └── debug
```

For using this template in your project, **clone** this repo and **rename** it to your project name.
```
git clone https://github.com/harikrishnamohann/Ctemplate.git
mv Ctemplate <your project name>
```

Oh and also don't forget to **remove git repository** by removing the `.git` folder

For **using** any of the **library** in the lib/ folder, you just have to include it in the src/main.c file. For example, 
```c
#include "../lib/strings.c"
int main() {
	String key = str_init("hello world");
	return 0;
}
```
A **How to use** section is defined at the top of each library file. You can refer that to learn how to use those libraries.

Thank you for taking a look at my silly stuffs. That's all `^-^`