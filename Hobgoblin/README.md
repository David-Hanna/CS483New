# Hobgoblin

## What Is Hobgoblin
Hobgoblin is a command line tool used to convert Wavefront OBJ files into a custom binary format to allow for quick parsing.

## Getting Started
Before using Hobgoblin, build the included Visual Studio solution.

## Using Hobgoblin
To use Hobgoblin, simply call navigate to the folder containing hobgoblin.exe and call run `./hobgoblin <filename>`.  For example, to convert cleopapa.obj, you would call `./hobgoblin ../../../Assets/Drivers/Cleopapa/cleopapa.obj`.  This will generate a .hobj in the same folder and with the same name as the original .obj file.

## File Format
Hobgoblin uses a pretty simple file format to make it easy to read data from .hobj files.

The format is as follows:
* MTL file name length :: size_t
* MTL file name :: string
* number of vertices :: size_t
* vertices :: float[]
* number of UVs :: size_t
* UVs :: float[]
* number of normals :: size_t
* normals :: float[]
* number of objects :: size_t
* Objects
  * object name length :: size_t
  * object name :: string
  * material name length :: size_t
  * material name :: string
  * number of faces :: size_t
  * faces
    * point 1 :: float[3]
    * point 2 :: float[3]
    * point 3 :: float[3]