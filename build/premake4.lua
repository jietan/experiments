solution "0MySolution"

	-- Multithreaded compiling
	if _ACTION == "vs2010" or _ACTION=="vs2008" then
		buildoptions { "/MP"  }
	end 
	
	newoption {
    trigger     = "with-nacl",
    description = "Enable Native Client build"
  }
  
	configurations {"Release", "Debug"}
	configuration "Release"
		flags { "Optimize", "StaticRuntime", "NoMinimalRebuild", "FloatFast"}
	configuration "Debug"
		flags { "Symbols", "StaticRuntime" , "NoMinimalRebuild", "NoEditAndContinue" ,"FloatFast"}
		
	platforms {"x32", "x64"}

	configuration "x64"		
		targetsuffix "_64"
	configuration {"x64", "debug"}
		targetsuffix "_x64_debug"
	configuration {"x64", "release"}
		targetsuffix "_x64"
	configuration {"x32", "debug"}
		targetsuffix "_debug"

	configuration{}

if not _OPTIONS["with-nacl"] then
		flags { "NoRTTI", "NoExceptions"}
		defines { "_HAS_EXCEPTIONS=0" }
		targetdir "../bin"
else
	targetdir "../bin_html"
end


	dofile ("findOpenCL.lua")
	dofile ("findDirectX11.lua")
	
	language "C++"
	
	location("./" .. _ACTION)
	

	include "../bullet2"	
	
if not _OPTIONS["with-nacl"] then

	include "../opencl/opengl_interop"
	include "../opencl/integration"
	include "../opencl/primitives/AdlTest"
	include "../rendering/GLSL_Instancing"
	include "../opencl/basic_initialize"


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

	include "../dynamics/testbed"
	include "../dynamics/position_based_dynamics"

	include "../jpeglib"
	
	include "../dynamics/corotational_fem"
	--include "../dynamics/nncg_test"

	include "../rendering/Gwen/Gwen"
	include "../rendering/Gwen/GwenOpenGLTest"

end

if _OPTIONS["with-nacl"] then
	include "../rendering/NativeClient"	
end	
