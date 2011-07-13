solution "0MySolution"

	configurations {"Release", "Debug"}
	configuration "Release"
		flags { "Optimize", "StaticRuntime", "NoRTTI", "NoExceptions"}
	configuration "Debug"
		flags { "Symbols", "StaticRuntime" , "NoRTTI", "NoExceptions"}
	platforms {"x32", "x64"}

	configuration "x64"		
		targetsuffix "_64"
	configuration {"x64", "debug"}
		targetsuffix "_x64_debug"
	configuration {"x64", "release"}
		targetsuffix "_x64"
	configuration {"x32", "debug"}
		targetsuffix "_debug"


	dofile ("findOpenCL.lua")
	dofile ("findDirectX11.lua")
	
	language "C++"
	location "build"
	targetdir "bin"

	include "../opencl/opengl_interop"
	include "../opencl/integration"
	include "../opencl/primitives/AdlTest"
	include "../rendering/GLSL_Instancing"
	include "../opencl/basic_initialize"

	include "../bullet2"

	include "../physics_effects/base_level"
	include "../physics_effects/low_level"
	include "../physics_effects/util"
	include "../physics_effects/sample_api_physics_effects/0_console"
	include "../physics_effects/sample_api_physics_effects/1_simple"
	include "../physics_effects/sample_api_physics_effects/2_stable"
	include "../physics_effects/sample_api_physics_effects/3_sleep"
	include "../physics_effects/sample_api_physics_effects/4_motion_type"
	include "../physics_effects/sample_api_physics_effects/5_raycast"
	include "../physics_effects/sample_api_physics_effects/6_joint"
		
	
	
	
	
	