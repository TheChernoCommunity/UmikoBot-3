# UmikoBot

## Contributing

### Pre-requisites

This project makes use of CMake, so install that. Everything else will be automatically installed on build.

### Setup

This repository uses Git Submodules, so clone it using:

```sh
git clone https://github.com/TheChernoCommunity/UmikoBot-3 --recurse-submodules
```

Alternatively, initialise submodules on an already cloned repository using:

```sh
git submodule update --init --recursive
```

### Building and running

Run `tools/build.sh` (Linux, preferred) or `tools\\build.bat` (Windows) from any directory. This will build the entire project (the first build will take a long time as it builds all dependencies), and will run it on a successful build.

You can find the executable in the `bin/` directory, run it from there if you're running it manually.

**Note:** You need to provide a token for the bot to use. Do this by setting the `UMIKO_DEBUG_TOKEN` environment variable in your shell before running the application.
