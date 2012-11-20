QtRpc2 - Simple to use Qt based RPC library.
============================================
QtRpc2 is a RPC library based on the Qt framework. It leverages Qt's meta object system to greatly simplify RPC calls, and significantly reduce boilerplate code.

##Features
* Simple to use
* Events and asynchronous function calls
* Simple error checking
* Token based authentication model
* Multiple transport modes (TCP, SSL, Named Pipe)
* Flexible threading model
* Service discovery


Documentation
----------------------
There really isn't any right now. Have a look at examples/basic_client and examples/basic_server for an example on how to get started. The other examples in the examples folder cover the major features of the library.

Build Instructions
-------------------------
### Requirements
* Qt 4.6 or above.
* CMake 2.6 or above.
* Bonjour, or Avahi on Linux.

on Debian or Ubuntu, this command will install the required dependencies.   
```
sudo apt-get install libqt4-dev cmake libavahi-client-dev libavahi-compat-libdnssd-dev
```

### Building
Check out the git repository with   
```
git://github.com/brendan0powers/QtRpc2.git
```

to build, create a build directory, and then run CMake.  

#### Linux, OS X  
```
cd QtRpc2/   
mkdir build   
cd build/  
cmake ../  
make
```

#### Windows  
```
cd QtRpc2/   
mkdir build   
cd build/  
cmake ../  -D
make
```

If qmake is not in your path, add the qmake path to the CMake command line.
```  -DQT_QMAKE_EXECUTABLE=C:\path\to\qmake.exe``` 

