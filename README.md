# msPlugins

A set of plugins for Autodesk Maya.

**_NOTE:_**  This repository is no longer actively maintained.

## _Dependencies_

This project depends upon [CMake](https://cmake.org/download/) to generate build files, which are used to automate the installation of a distributable Maya module.

## _Build Targets_

Source code is organised into project directories, where each project results in one or more build targets and may contain a number of sub-projects.

If a project has dependencies in a parent or sibling directory these will be automatically resolved by the build process.

Installation of build targets results in a distributable Maya module containing plugins, scripts and templates.

This process is designed to reduce the administrative burden of distribution by automating aspects of the build, environment setup and dependency resolution.

## _Build Definitions_

#### Targets

The following arguments can be used to generate multiple build targets (default is **OFF**):

```
-DBUILD_NODES=ON
```

The following arguments can be used to generate individual build targets (default is **OFF**):

```
-DBUILD_ARRAY_NODES=ON
-DBUILD_HRC_NODES=ON
-DBUILD_COMPOENT_NODES=ON
-DBUILD_MATH_NODES=ON
```

#### Customization

The following argument can be used to set a custom prefix for any generated node types (default is **mrs_**):

```
-DNODE_NAME_PREFIX=yourCustomPrefix_
```

#### Maya

The following argument can be used to set which Maya version to build for (default is **2019**):


```
-DMAYA_VERSION=2017
```

The build will look for Maya in the default installation directory of each platform, however this can be changed as follows:

```
-DMAYA_LOCATION_BASE=<absolute path to>/autodesk/
```

#### Installation

The build will install targets into the `/dist` folder under the project root, however this can be changed as follows:

```
-DDIST_INSTALL_DIR=<absolute path to>/installation/
```

The build will generate certain targets which are used as internal dependencies.

Installation of these targets is optional and can be disabled using the following argument (default is **OFF**):

```
-DDISABLE_INTERNAL_TARGET_INSTALLATION=ON
```

#### Developer

The build will install internal library dependencies into the `/lib` folder under the project root, however this can be changed as follows:

```
-DLIB_INSTALL_DIR=<absolute path to>/libs/
```

The build will install internal include file dependencies into the `/include` folder under the project root, however this can be changed as follows:

```
-DINCLUDE_INSTALL_DIR=<absolute path to>/headers/
```

## _Building / Installing_

#### Windows - Method 1

Clone the repository and navigate to the cloned directory, then run the following commands to create and enter the build directory:

```
mkdir build
cd build
```

Specify the build system and the targets for which CMake should generate configuration files, then provide the location of the root `CMakeLists.txt` file.

```
cmake -G "Visual Studio 15 2017 Win64" -DBUILD_NODES=ON ..
```

To build and install each target, simply open the generated solution file and build the **INSTALL** target.

#### Windows - Method 2

Open the cloned repository in Visual Studio by choosing **File | Open | Folder**.

Edit the `cmakeCommandArgs` variable in the `CMakeSettings.json` file to contain the necessary build definitions.

Generate a new cache by choosing **CMake | Cache | Generate | plugins**.

Build and install each target by choosing **CMake | Install | plugins**.

#### OSX / Linux

As per above, run these commands to create and enter a build directory.

```
mkdir build
cd build
```

The following command generates the necessary configuration files as well as a makefile for the specified targets:

```
cmake -G "CodeBlocks - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_NODES=ON ..
```

The makefile can be used with the Make build system to build and install each target as follows:

```
make
make install
```

## _Distribution_

#### Method 1

Having installed the Maya module into the `/dist` folder, create or locate a text file named `Maya.env`, usually in the following directory:

* **Windows**:<br/>
`<drive>:\Users\<username>\Documents\maya\<version>`

* **Mac OS X**:<br/>
`~/Library/Preferences/Autodesk/maya/<version>`

* **Linux**:<br/>
`~/maya/<version>`

Configure the `MAYA_MODULE_PATH` variable within `Maya.env`, using a semicolon `;` on Windows or a colon `:` on Linux and Mac OS X to separate existing paths:

* **Windows**:<br/>
`MAYA_MODULE_PATH = <absolute path to>\msPlugins\dist\module`

* **Linux / Mac OS X**:<br/>
`MAYA_MODULE_PATH = <absolute path to>/msPlugins/dist/module`

#### Method 2

Having installed the Maya module into the `/dist` folder, move the `msPlugins.mod` file from the `/module` sub-directory to an existing directory in the `MAYA_MODULE_PATH`, for example:

* **Windows**:<br/>
`<drive>:\Users\<username>\Documents\maya\<version>\modules`

* **Mac OS X**:<br/>
`~/Library/Preferences/Autodesk/maya/<version>/modules`

* **Linux**:<br/>
`~/maya/<version>/modules`

Open the `msPlugins.mod` file and change the relative path of the module directory to an absolute path, so that this line:

`+ MAYAVERSION:<maya version> msPlugins <project version> ./`

Now looks like this line:

`+ MAYAVERSION:<maya version> msPlugins <project version> <absolute path to>/msPlugins/dist/module/`