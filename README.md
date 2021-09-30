my-gcc-plugin
=============

This is a simple GCC Gimple plugin fun project.
It throws warnings when a function return value is unused.

Build
=====

```
mkdir Debug
cd Debug
cmake ..
make
```

Run
===
```
g++ -c -fplugin=./my_gcc_plugin.so -c src/examples/test-warn.cpp
```