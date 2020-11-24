# xcsg - xml based constructive solid geometry
xcsg is both a file format and an application. For the file format definition, please see the [xcsg wiki](https://github.com/arnholm/xcsg/wiki) for detailed information.

## The xcsg application 

The xcsg application takes an input .xcsg file, compiles its data into a solid model, and exports it to common surface mesh file formats, including STL. The application uses the [Carve library](https://github.com/arnholm/carve) for boolean operations.

    $ xcsg --help
   
	xcsg command line options & arguments (v1.5-00):
	  -h [ --help ]         Show this help message.
	  -v [ --version ]      Show program version (numeric part).
	  --amf                 AMF output format (Additive Manufacturing Format)
	  --csg                 CSG output format (OpenSCAD)
	  --dxf                 DXF output format (AutoCAD DXF - 2D only)
	  --svg                 SVG output format (Scalar Vector Graphics - 2D only)
	  --stl                 STL output format (STereoLitography)
	  --astl                STL output format (STereoLitography) - ASCII
	  --obj                 OBJ output format (Wavefront format)
	  --off                 OFF output format (Geomview Object File Format)
	  --export_dir arg      Export output files to directory
	  --max_bool arg        Max number of booleans allowed
	  --sec_tol arg         Secant tolerance when importing OpenSCAD csg (0.05)
	  --fullpath            Show full file paths. 
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

