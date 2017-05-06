# xcsg - xml based constructive solid geometry
xcsg is both a file format and an application. For the file format definition, please see the [xcsg wiki](https://github.com/arnholm/xcsg/wiki) for detailed information.

## The xcsg application 

The xcsg application takes an input .xcsg file, compiles its data into a solid model, and exports it to common surface mesh file formats, including STL.

    $ xcsg --help
    xcsg version v1.0-00
    
    xcsg command line options & arguments:
      -h [ --help ]         Show this help message.
      -v [ --version ]      Show program version.
      --amf                 AMF output file format (Additive Manufacturing Format)
      --csg                 CSG output file format (OpenSCAD)
      --stl                 STL output file format (STereoLitography)
      --astl                STL output file format (STereoLitography) - ASCII
      --obj                 OBJ output file format (Wavefront format)
      --off                 OFF output file format (Geomview Object File Format)
      <xcsg-file>           path to input .xcsg file (required)


### example
To compute the difference between a cube and a sphere and store the result as STL

    $ xcsg --stl difference3d.xcsg

The file difference3d.xcsg:
```xml
<?xml version="1.0" encoding="utf-8"?>
<xcsg version="1.0">
    <difference3d>
        <cube size="50" center="true"/>
        <sphere r="33"/>
    </difference3d>
</xcsg>
```
The resulting STL becomes:
![](https://raw.githubusercontent.com/wiki/arnholm/xcsg/images/difference3d.png)

