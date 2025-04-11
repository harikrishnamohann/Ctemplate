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
└── target	:::::::::::: executable lives here
    └── debug
```

For using this template in your project, clone this repo and rename it to your project name.
```
git clone https://github.com/harikrishnamohann/Ctemplate.git
mv Ctemplate <your project name>
```

Oh and also don't forget to remove the git history by removing the `.git` folder

Thank you for taking a look at my silly stuffs. That's all `^-^`