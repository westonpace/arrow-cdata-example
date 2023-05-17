# Arrow-C++ / PyBind11 / Pyarrow Example

This repository is a minimal example showing how to build your own C++ module, which
relies on Arrow-C++, exposes python bindings with pybind11, and is compatible with
pyarrow.

It uses the C data interface to convert arrays from pyarrow to Arrow-C++.  This is a
zero copy stable ABI which allows us to decouple ourselves from the specifics of the
pyarrow build.

## Building

You will need Arrow-C++ and pybind11 installed in some way.  For example, if you use conda
then you will need to run:

```bash
conda install arrow-cpp pybind11 pyarrow
```

If you are using system dependencies then you will probably need to install arrow-cpp and pybind11
with your package manager.  You do not need pyarrow to be installed for development of the module
but you will need it installed to execute or test the module.

Once you have these dependencies you can go ahead and compile the application

```bash
mkdir build
cd build
cmake ..
make
```

The build will create a file with a name like `MyModule.cpython-311-x86_64-linux-gnu.so`.  This is your
python module.  Once this file is on your PYTHONPATH (or in the current directory) then you can run the
example script `example.py`.  For example, if you've just built the project (and are still in the build
directory):

```bash
cp ../example.py .
python example.py
```
