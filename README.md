
# StreamIndex
> Demo only for string and audio/phonome.

StreamIndex is an open-source stream data index project. It will help you to implement a light stream data index with functions of insertion, query, update and so on.

## Getting started

### Prerequisites

**Install Python Dependency** 

```shell
sudo pip install scipy numpy
```
**Install Essentia**

In order to process the audio, we need to install [Essentia](http://essentia.upf.edu/documentation/installing.html)

**Linux user** can use the following instruction to install the dependencies.

```shell
sudo apt-get install build-essential libyaml-dev libfftw3-dev libavcodec-dev libavformat-dev libavutil-dev libavresample-dev python-dev libsamplerate0-dev libtag1-dev
sudo apt-get install python-numpy-dev python-numpy python-yaml
sudo apt-get install autoconf
```

Then users can compile Essentia.

```shell
git clone https://github.com/MTG/essentia.git
cd essentia-master/
./waf configure --mode=release --build-static --with-python --with-cpptests --with-examples --with-vamp
./waf
./waf install
```
**Install C++ Dependency**

Download cpp-btree and install it from [Google-CppBtree](https://code.google.com/archive/p/cpp-btree/)

```shell
cd cpp-btree-1.0.1/
cmake . -Dbuild_tests=ON
```

In order to run string demo, you may have to install [CppJieba](https://github.com/yanyiwu/cppjieba)

```shell
git clone --depth=10 --branch=master git://github.com/yanyiwu/cppjieba.git
cd cppjieba
mkdir build
cd build
cmake ..
make
```
For sake of using snapshot, [SSDB for C++](https://github.com/ideawu/ssdb) is fundamental.

```shell
wget --no-check-certificate https://github.com/ideawu/ssdb/archive/master.zip
unzip master
cd ssdb-master
make
sudo make install
```
Before running the index, we should run the server in advance.

```shell
./ssdb-server ssdb.conf &
```

### Building

The project is compiled by CMake 1.7. First, we download the source code and enter the directory to build.

```shell
git clone https://github.com/xingyang-liu/StreamAudioIndex.git
cd StreamAudioIndex-master/
```
Then we create a directory to build the project.

```shell
mkdir build/
cmake ..
make
```
Then we get a excutable file *StreamAudioIndex.a*

### Deploying / Publishing
give instructions on how to build and release a new version
In case there's some step you have to take that publishes this project to a
server, this is the right time to state it.

```shell
packagemanager deploy your-project -s server.com -u username -p password
```

And again you'd need to tell what the previous code actually does.

## Versioning

We can maybe use [SemVer](http://semver.org/) for versioning. For the versions available, see the [link to tags on this repository](/tags).

## Configuration

Here you should write what are all of the configurations a user can enter when
using the project.

## Tests

Describe and show how to run the tests with code examples.
Explain what these tests test and why.

```shell
Give an example
```

## Architecture
<img src="./image/0001.jpg" width="35%" height="35%">
Based on Log-Structured Inverted Indices, we arrange the sum of terms in indices in accordance with certain ratio(2 in demo).....

## Api Reference

If the api is external, link to api documentation. If not describe your api including authentication methods as well as explaining all the endpoints with their required parameters.

## Database

Explaining what database (and version) has been used. Provide download links.
Documents your database design and schemas, relations etc... 

## Licensing

StreamIndex is licensed under [New BSD License](http://opensource.org/licenses/BSD-3-Clause), a very flexible license to use.
