-- premake5 script, genrated by Code::Blocks plugin premake5cb by cacb
workspace "XCSG"
	location "buildpm5"
	configurations { "debug","release" } 

	project "qhull"
		location "buildpm5/qhull"
		architecture  ( "x86_64" ) 
		cppdialect  ( "c++17" ) 
		defines ( "QHULL_IMPLEMENTATION" ) 
		exceptionhandling  ( "on" ) 
		includedirs ( "qhull" ) 
		language  ( "c++" ) 
		pic  ( "on" ) 
		rtti  ( "on" ) 
		staticruntime  ( "off" ) 

		-- 'files' paths are relative to premake file
		files {
			"qhull/libqhull_r/geom2_r.c"
			,"qhull/libqhull_r/geom_r.c"
			,"qhull/libqhull_r/geom_r.h"
			,"qhull/libqhull_r/global_r.c"
			,"qhull/libqhull_r/io_r.c"
			,"qhull/libqhull_r/io_r.h"
			,"qhull/libqhull_r/libqhull_r.c"
			,"qhull/libqhull_r/libqhull_r.h"
			,"qhull/libqhull_r/mem_r.c"
			,"qhull/libqhull_r/mem_r.h"
			,"qhull/libqhull_r/merge_r.c"
			,"qhull/libqhull_r/merge_r.h"
			,"qhull/libqhull_r/poly2_r.c"
			,"qhull/libqhull_r/poly_r.c"
			,"qhull/libqhull_r/poly_r.h"
			,"qhull/libqhull_r/qhull_ra.h"
			,"qhull/libqhull_r/qset_r.c"
			,"qhull/libqhull_r/qset_r.h"
			,"qhull/libqhull_r/random_r.c"
			,"qhull/libqhull_r/random_r.h"
			,"qhull/libqhull_r/rboxlib_r.c"
			,"qhull/libqhull_r/stat_r.c"
			,"qhull/libqhull_r/stat_r.h"
			,"qhull/libqhull_r/user_r.c"
			,"qhull/libqhull_r/user_r.h"
			,"qhull/libqhull_r/usermem_r.c"
			,"qhull/libqhullcpp/Coordinates.cpp"
			,"qhull/libqhullcpp/Coordinates.h"
			,"qhull/libqhullcpp/PointCoordinates.cpp"
			,"qhull/libqhullcpp/PointCoordinates.h"
			,"qhull/libqhullcpp/Qhull.cpp"
			,"qhull/libqhullcpp/Qhull.h"
			,"qhull/libqhullcpp/QhullError.h"
			,"qhull/libqhullcpp/QhullFacet.cpp"
			,"qhull/libqhullcpp/QhullFacet.h"
			,"qhull/libqhullcpp/QhullFacetList.cpp"
			,"qhull/libqhullcpp/QhullFacetList.h"
			,"qhull/libqhullcpp/QhullFacetSet.cpp"
			,"qhull/libqhullcpp/QhullFacetSet.h"
			,"qhull/libqhullcpp/QhullHyperplane.cpp"
			,"qhull/libqhullcpp/QhullHyperplane.h"
			,"qhull/libqhullcpp/QhullIterator.h"
			,"qhull/libqhullcpp/QhullLinkedList.h"
			,"qhull/libqhullcpp/QhullPoint.cpp"
			,"qhull/libqhullcpp/QhullPoint.h"
			,"qhull/libqhullcpp/QhullPointSet.cpp"
			,"qhull/libqhullcpp/QhullPointSet.h"
			,"qhull/libqhullcpp/QhullPoints.cpp"
			,"qhull/libqhullcpp/QhullPoints.h"
			,"qhull/libqhullcpp/QhullQh.cpp"
			,"qhull/libqhullcpp/QhullQh.h"
			,"qhull/libqhullcpp/QhullRidge.cpp"
			,"qhull/libqhullcpp/QhullRidge.h"
			,"qhull/libqhullcpp/QhullSet.cpp"
			,"qhull/libqhullcpp/QhullSet.h"
			,"qhull/libqhullcpp/QhullSets.h"
			,"qhull/libqhullcpp/QhullStat.cpp"
			,"qhull/libqhullcpp/QhullStat.h"
			,"qhull/libqhullcpp/QhullVertex.cpp"
			,"qhull/libqhullcpp/QhullVertex.h"
			,"qhull/libqhullcpp/QhullVertexSet.cpp"
			,"qhull/libqhullcpp/QhullVertexSet.h"
			,"qhull/libqhullcpp/RboxPoints.cpp"
			,"qhull/libqhullcpp/RboxPoints.h"
			,"qhull/libqhullcpp/RoadError.cpp"
			,"qhull/libqhullcpp/RoadError.h"
			,"qhull/libqhullcpp/RoadLogEvent.cpp"
			,"qhull/libqhullcpp/RoadLogEvent.h"
			,"qhull/libqhullcpp/functionObjects.h"
			,"qhull/qhull2d.cpp"
			,"qhull/qhull2d.h"
			,"qhull/qhull3d.cpp"
			,"qhull/qhull3d.h"
			,"qhull/qhull_config.h"
			,"qhull/qvec3d.cpp"
			,"qhull/qvec3d.h"
			}

		filter { "configurations:debug" }
			defines  ( "DEBUG" ) 
			kind ( "StaticLib" ) 
			symbols  ( "on" ) 
		filter { }

		filter { "configurations:release" }
			defines  ( "NDEBUG" ) 
			kind ( "StaticLib" ) 
			optimize  ( "on" ) 
		filter { }

	project "dmesh"
		location "buildpm5/dmesh"
		architecture  ( "x86_64" ) 
		cppdialect  ( "c++17" ) 
		defines ( "DMESH_IMPLEMENTATION" ) 
		exceptionhandling  ( "on" ) 
		includedirs ( "dmesh" ) 
		language  ( "c++" ) 
		pic  ( "on" ) 
		rtti  ( "on" ) 
		staticruntime  ( "off" ) 

		-- 'files' paths are relative to premake file
		files {
			"dmesh/dbox2d.cpp"
			,"dmesh/dbox2d.h"
			,"dmesh/dcircle.cpp"
			,"dmesh/dcircle.h"
			,"dmesh/dcoedge.cpp"
			,"dmesh/dcoedge.h"
			,"dmesh/dedge.cpp"
			,"dmesh/dedge.h"
			,"dmesh/dentity.cpp"
			,"dmesh/dentity.h"
			,"dmesh/dline2d.cpp"
			,"dmesh/dline2d.h"
			,"dmesh/dloop.cpp"
			,"dmesh/dloop.h"
			,"dmesh/dloop_optimizer.cpp"
			,"dmesh/dloop_optimizer.h"
			,"dmesh/dmesh.cpp"
			,"dmesh/dmesh.h"
			,"dmesh/dpos2d.cpp"
			,"dmesh/dpos2d.h"
			,"dmesh/dprofile.cpp"
			,"dmesh/dprofile.h"
			,"dmesh/dtriangle.cpp"
			,"dmesh/dtriangle.h"
			,"dmesh/dvec2d.cpp"
			,"dmesh/dvec2d.h"
			,"dmesh/dvertex.cpp"
			,"dmesh/dvertex.h"
			}

		filter { "configurations:debug" }
			defines  ( "DEBUG" ) 
			kind ( "StaticLib" ) 
			symbols  ( "on" ) 
		filter { }

		filter { "configurations:release" }
			defines  ( "NDEBUG" ) 
			kind ( "StaticLib" ) 
			optimize  ( "on" ) 
		filter { }

	project "tmesh"
		location "buildpm5/tmesh"
		architecture  ( "x86_64" ) 
		cppdialect  ( "c++17" ) 
		defines ( "TMESH_IMPLEMENTATION" ) 
		exceptionhandling  ( "on" ) 
		includedirs ( "tmesh" ) 
		language  ( "c++" ) 
		pic  ( "on" ) 
		rtti  ( "on" ) 
		staticruntime  ( "off" ) 

		-- 'files' paths are relative to premake file
		files {
			"tmesh/libtess2/Include/tesselator.h"
			,"tmesh/libtess2/Source/bucketalloc.c"
			,"tmesh/libtess2/Source/bucketalloc.h"
			,"tmesh/libtess2/Source/dict.c"
			,"tmesh/libtess2/Source/dict.h"
			,"tmesh/libtess2/Source/geom.c"
			,"tmesh/libtess2/Source/geom.h"
			,"tmesh/libtess2/Source/mesh.c"
			,"tmesh/libtess2/Source/mesh.h"
			,"tmesh/libtess2/Source/priorityq.c"
			,"tmesh/libtess2/Source/priorityq.h"
			,"tmesh/libtess2/Source/sweep.c"
			,"tmesh/libtess2/Source/sweep.h"
			,"tmesh/libtess2/Source/tess.c"
			,"tmesh/libtess2/Source/tess.h"
			,"tmesh/tmesh_config.h"
			}

		filter { "configurations:debug" }
			defines  ( "DEBUG" ) 
			kind ( "StaticLib" ) 
			symbols  ( "on" ) 
		filter { }

		filter { "configurations:release" }
			defines  ( "NDEBUG" ) 
			kind ( "StaticLib" ) 
			optimize  ( "on" ) 
		filter { }

	project "csplines"
		location "buildpm5/csplines"
		architecture  ( "x86_64" ) 
		cppdialect  ( "c++17" ) 
		defines ( "CSPLINES_IMPLEMENTATION" ) 
		exceptionhandling  ( "on" ) 
		includedirs ( "csplines" ) 
		language  ( "c++" ) 
		pic  ( "on" ) 
		rtti  ( "on" ) 
		staticruntime  ( "off" ) 

		-- 'files' paths are relative to premake file
		files {
			"csplines/ap/ap.cpp"
			,"csplines/ap/ap.h"
			,"csplines/ap/apvt.h"
			,"csplines/ap/spline3.cpp"
			,"csplines/ap/spline3.h"
			,"csplines/csplines_config.h"
			,"csplines/spline_path.cpp"
			,"csplines/spline_path.h"
			}

		filter { "configurations:debug" }
			defines  ( "DEBUG" ) 
			kind ( "StaticLib" ) 
			symbols  ( "on" ) 
		filter { }

		filter { "configurations:release" }
			defines  ( "NDEBUG" ) 
			kind ( "StaticLib" ) 
			optimize  ( "on" ) 
		filter { }

	project "csg_parser"
		location "buildpm5/csg_parser"
		architecture  ( "x86_64" ) 
		cppdialect  ( "c++17" ) 
		defines ( "CSG_PARSER_IMPLEMENTATION" ) 
		exceptionhandling  ( "on" ) 
		includedirs ( "csg_parser" ) 
		language  ( "c++" ) 
		pic  ( "on" ) 
		rtti  ( "on" ) 
		staticruntime  ( "off" ) 

		-- 'files' paths are relative to premake file
		files {
			"csg_parser/cf_xmlNode.cpp"
			,"csg_parser/cf_xmlNode.h"
			,"csg_parser/cf_xmlTree.cpp"
			,"csg_parser/cf_xmlTree.h"
			,"csg_parser/csg_matrix.cpp"
			,"csg_parser/csg_matrix.h"
			,"csg_parser/csg_node.cpp"
			,"csg_parser/csg_node.h"
			,"csg_parser/csg_parser.cpp"
			,"csg_parser/csg_parser.h"
			,"csg_parser/csg_scalar.cpp"
			,"csg_parser/csg_scalar.h"
			,"csg_parser/csg_value.cpp"
			,"csg_parser/csg_value.h"
			,"csg_parser/csg_vector.cpp"
			,"csg_parser/csg_vector.h"
			}

		filter { "configurations:debug" }
			defines  ( "DEBUG" ) 
			kind ( "StaticLib" ) 
			symbols  ( "on" ) 
		filter { }

		filter { "configurations:release" }
			defines  ( "NDEBUG" ) 
			kind ( "StaticLib" ) 
			optimize  ( "on" ) 
		filter { }

	project "xcsg"
		location "buildpm5/xcsg"
		architecture  ( "x86_64" ) 
		cppdialect  ( "c++17" ) 
		dependson { "csg_parser","csplines","dmesh","qhull","tmesh" } 
		exceptionhandling  ( "on" ) 
		includedirs { ".","csg_parser","csplines","dmesh","qhull","tmesh","xcsg" } 
		language  ( "c++" ) 
		pic  ( "on" ) 
		rtti  ( "on" ) 
		staticruntime  ( "off" ) 

		-- 'files' paths are relative to premake file
		files {
			"xcsg/amf_file.cpp"
			,"xcsg/amf_file.h"
			,"xcsg/boolean_timer.cpp"
			,"xcsg/boolean_timer.h"
			,"xcsg/boost_command_line.cpp"
			,"xcsg/boost_command_line.h"
			,"xcsg/carve_boolean.cpp"
			,"xcsg/carve_boolean.h"
			,"xcsg/carve_boolean_thread.cpp"
			,"xcsg/carve_boolean_thread.h"
			,"xcsg/carve_mesh_thread.cpp"
			,"xcsg/carve_mesh_thread.h"
			,"xcsg/carve_minkowski_hull.cpp"
			,"xcsg/carve_minkowski_hull.h"
			,"xcsg/carve_minkowski_thread.cpp"
			,"xcsg/carve_minkowski_thread.h"
			,"xcsg/carve_triangulate.cpp"
			,"xcsg/carve_triangulate.h"
			,"xcsg/carve_triangulate_face.cpp"
			,"xcsg/carve_triangulate_face.h"
			,"xcsg/clipper_boolean.cpp"
			,"xcsg/clipper_boolean.h"
			,"xcsg/clipper_csg/clipper.cpp"
			,"xcsg/clipper_csg/clipper_csg_config.h"
			,"xcsg/clipper_csg/clipper_offset.cpp"
			,"xcsg/clipper_csg/clipper_offset.h"
			,"xcsg/clipper_csg/clipper_profile.cpp"
			,"xcsg/clipper_csg/clipper_profile.h"
			,"xcsg/clipper_csg/contour2d.cpp"
			,"xcsg/clipper_csg/contour2d.h"
			,"xcsg/clipper_csg/dmesh_adapter.cpp"
			,"xcsg/clipper_csg/dmesh_adapter.h"
			,"xcsg/clipper_csg/polygon2d.cpp"
			,"xcsg/clipper_csg/polygon2d.h"
			,"xcsg/clipper_csg/polymesh2d.cpp"
			,"xcsg/clipper_csg/polymesh2d.h"
			,"xcsg/clipper_csg/polyset2d.cpp"
			,"xcsg/clipper_csg/polyset2d.h"
			,"xcsg/clipper_csg/tmesh_adapter.cpp"
			,"xcsg/clipper_csg/tmesh_adapter.h"
			,"xcsg/clipper_csg/vmap2d.cpp"
			,"xcsg/clipper_csg/vmap2d.h"
			,"xcsg/dxf_file.cpp"
			,"xcsg/dxf_file.h"
			,"xcsg/extrude_mesh.cpp"
			,"xcsg/extrude_mesh.h"
			,"xcsg/geodesic_sphere.cpp"
			,"xcsg/geodesic_sphere.h"
			,"xcsg/main.cpp"
			,"xcsg/mesh_utils.cpp"
			,"xcsg/mesh_utils.h"
			,"xcsg/openscad_csg.cpp"
			,"xcsg/openscad_csg.h"
			,"xcsg/out_triangles.cpp"
			,"xcsg/out_triangles.h"
			,"xcsg/polymesh3d.cpp"
			,"xcsg/polymesh3d.h"
			,"xcsg/primitives2d.cpp"
			,"xcsg/primitives2d.h"
			,"xcsg/primitives3d.cpp"
			,"xcsg/primitives3d.h"
			,"xcsg/project_mesh.cpp"
			,"xcsg/project_mesh.h"
			,"xcsg/safe_queue.h"
			,"xcsg/std_filename.cpp"
			,"xcsg/std_filename.h"
			,"xcsg/svg_file.cpp"
			,"xcsg/svg_file.h"
			,"xcsg/sweep_mesh.cpp"
			,"xcsg/sweep_mesh.h"
			,"xcsg/sweep_path.cpp"
			,"xcsg/sweep_path.h"
			,"xcsg/sweep_path_linear.cpp"
			,"xcsg/sweep_path_linear.h"
			,"xcsg/sweep_path_rotate.cpp"
			,"xcsg/sweep_path_rotate.h"
			,"xcsg/sweep_path_spline.cpp"
			,"xcsg/sweep_path_spline.h"
			,"xcsg/sweep_path_transform.cpp"
			,"xcsg/sweep_path_transform.h"
			,"xcsg/tin_mesh.cpp"
			,"xcsg/tin_mesh.h"
			,"xcsg/version.h"
			,"xcsg/xcircle.cpp"
			,"xcsg/xcircle.h"
			,"xcsg/xcone.cpp"
			,"xcsg/xcone.h"
			,"xcsg/xcsg_factory.cpp"
			,"xcsg/xcsg_factory.h"
			,"xcsg/xcsg_main.cpp"
			,"xcsg/xcsg_main.h"
			,"xcsg/xcube.cpp"
			,"xcsg/xcube.h"
			,"xcsg/xcuboid.cpp"
			,"xcsg/xcuboid.h"
			,"xcsg/xcylinder.cpp"
			,"xcsg/xcylinder.h"
			,"xcsg/xdifference2d.cpp"
			,"xcsg/xdifference2d.h"
			,"xcsg/xdifference3d.cpp"
			,"xcsg/xdifference3d.h"
			,"xcsg/xface.cpp"
			,"xcsg/xface.h"
			,"xcsg/xfill2d.cpp"
			,"xcsg/xfill2d.h"
			,"xcsg/xhull2d.cpp"
			,"xcsg/xhull2d.h"
			,"xcsg/xhull3d.cpp"
			,"xcsg/xhull3d.h"
			,"xcsg/xintersection2d.cpp"
			,"xcsg/xintersection2d.h"
			,"xcsg/xintersection3d.cpp"
			,"xcsg/xintersection3d.h"
			,"xcsg/xlinear_extrude.cpp"
			,"xcsg/xlinear_extrude.h"
			,"xcsg/xminkowski2d.cpp"
			,"xcsg/xminkowski2d.h"
			,"xcsg/xminkowski3d.cpp"
			,"xcsg/xminkowski3d.h"
			,"xcsg/xoffset2d.cpp"
			,"xcsg/xoffset2d.h"
			,"xcsg/xpolygon.cpp"
			,"xcsg/xpolygon.h"
			,"xcsg/xpolyhedron.cpp"
			,"xcsg/xpolyhedron.h"
			,"xcsg/xprojection2d.cpp"
			,"xcsg/xprojection2d.h"
			,"xcsg/xrectangle.cpp"
			,"xcsg/xrectangle.h"
			,"xcsg/xrotate_extrude.cpp"
			,"xcsg/xrotate_extrude.h"
			,"xcsg/xshape.cpp"
			,"xcsg/xshape.h"
			,"xcsg/xshape2d.cpp"
			,"xcsg/xshape2d.h"
			,"xcsg/xshape2d_collector.cpp"
			,"xcsg/xshape2d_collector.h"
			,"xcsg/xsolid.cpp"
			,"xcsg/xsolid.h"
			,"xcsg/xsolid_collector.cpp"
			,"xcsg/xsolid_collector.h"
			,"xcsg/xsphere.cpp"
			,"xcsg/xsphere.h"
			,"xcsg/xspline_path.cpp"
			,"xcsg/xspline_path.h"
			,"xcsg/xsquare.cpp"
			,"xcsg/xsquare.h"
			,"xcsg/xsweep.cpp"
			,"xcsg/xsweep.h"
			,"xcsg/xtin_model.cpp"
			,"xcsg/xtin_model.h"
			,"xcsg/xtmatrix.cpp"
			,"xcsg/xtmatrix.h"
			,"xcsg/xtransform_extrude.cpp"
			,"xcsg/xtransform_extrude.h"
			,"xcsg/xunion2d.cpp"
			,"xcsg/xunion2d.h"
			,"xcsg/xunion3d.cpp"
			,"xcsg/xunion3d.h"
			}

		filter { "configurations:debug" }
			defines  ( "DEBUG" ) 
			kind ( "ConsoleApp" ) 
			-- When linking within workspace, 'links' refer to project name.
			links { "carve","csg_parser","csplines","dmesh","qhull","tmesh" } 
			symbols  ( "on" ) 
		filter { }

		filter { "configurations:release" }
			defines  ( "NDEBUG" ) 
			kind ( "ConsoleApp" ) 
			-- When linking within workspace, 'links' refer to project name.
			links { "carve","csg_parser","csplines","dmesh","qhull","tmesh" } 
			optimize  ( "on" ) 
		filter { }
