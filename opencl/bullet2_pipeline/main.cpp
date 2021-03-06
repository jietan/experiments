/*
Copyright (c) 2012 Advanced Micro Devices, Inc.  

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
//Originally written by Erwin Coumans

//
//#include "vld.h"

#include "ShapeData.h"

#ifndef __APPLE__
#include <GL/glew.h>
#endif

#include "physics_func.h"
#include "btBulletDynamicsCommon.h"


#include "../../rendering/rendertest/GLInstancingRenderer.h"

#ifdef __APPLE__
#include "../../rendering/rendertest/MacOpenGLWindow.h"
#else
#include "../../rendering/rendertest/Win32OpenGLWindow.h"
#endif


#include "LinearMath/btQuickprof.h"
#include "LinearMath/btQuaternion.h"

#include "../../opencl/gpu_rigidbody_pipeline/CommandlineArgs.h"

bool printStats = false;
bool pauseSimulation = false;
bool shootObject = false;
//extern btVector3 m_cameraPosition;
//extern btVector3 m_cameraTargetPosition;


bool useInterop = false;


const char* fileName="../../bin/1000 stack.bullet";
void Usage()
{
	printf("\nprogram.exe [--pause_simulation=<0 or 1>] [--load_bulletfile=test.bullet] [--enable_interop=<0 or 1>] [--enable_gpusap=<0 or 1>] [--enable_convexheightfield=<0 or 1>] [--enable_static=<0 or 1>] [--x_dim=<int>] [--y_dim=<num>] [--z_dim=<int>] [--x_gap=<float>] [--y_gap=<float>] [--z_gap=<float>]\n"); 
};


static float sLocalTime=0.f;
static float sFixedTimeStep=1.f/60.f;

void stepSimulation(float dt)
{
	int maxSubSteps = 10;
	int numSimulationSubSteps = 0;
	if (maxSubSteps)
	{
		//fixed timestep with interpolation
		sLocalTime += dt;
		if (sLocalTime >= sFixedTimeStep)
		{
			numSimulationSubSteps = int( sLocalTime / sFixedTimeStep);
			sLocalTime -= numSimulationSubSteps * sFixedTimeStep;
		}
		if (numSimulationSubSteps)
		{
			//clamp the number of substeps, to prevent simulation grinding spiralling down to a halt
			int clampedSimulationSteps = (numSimulationSubSteps > maxSubSteps)? maxSubSteps : numSimulationSubSteps;
			for (int i=0;i<clampedSimulationSteps;i++)
			{
				physics_simulate();
			}
		} 
	}
}


void graphics_from_physics(GLInstancingRenderer& renderer, bool syncTransformsOnly)
{

    int cubeShapeIndex  = -1;
	int strideInBytes = sizeof(float)*9;
    
	if (!syncTransformsOnly)
	{
		int numVertices = sizeof(cube_vertices)/strideInBytes;
		int numIndices = sizeof(cube_indices)/sizeof(int);
		cubeShapeIndex = renderer.registerShape(&cube_vertices[0],numVertices,cube_indices,numIndices);
	}
    

    
    int numColObj = physics_get_num_rigidbodies();
    int curGraphicsIndex = 0;
    
    for (int i=0;i<numColObj;i++)
    {
        btCollisionObject* colObj = physics_get_collision_object(i);

        btVector3 pos = colObj->getWorldTransform().getOrigin();
        btQuaternion orn = colObj->getWorldTransform().getRotation();
        
        float position[4] = {pos.getX(),pos.getY(),pos.getZ(),0.f};
        float orientation[4] = {orn.getX(),orn.getY(),orn.getZ(),orn.getW()};
        float color[4] = {0,0,0,1};
        btVector3 localScaling = colObj->getCollisionShape()->getLocalScaling();
        
       
        if (colObj->isStaticOrKinematicObject())
        {
            color[0]=1.f;
        }else
        {
            color[1]=1.f;
        }
        
        switch (colObj->getCollisionShape()->getShapeType())
        {
            case BOX_SHAPE_PROXYTYPE:
            {
                btBoxShape* box = (btBoxShape*)colObj->getCollisionShape();

                btVector3 halfExtents = box->getHalfExtentsWithMargin();
                
                float cubeScaling[4] = {halfExtents.getX(),halfExtents.getY(), halfExtents.getZ(),1};
                
                if (!syncTransformsOnly)
                {
                    renderer.registerGraphicsInstance(cubeShapeIndex,position,orientation,color,cubeScaling);
                }
                else
                {
                    renderer.writeSingleInstanceTransformToCPU(position,orientation,curGraphicsIndex);
                
                }
                
                curGraphicsIndex++;
            }
            break;
                
            default:
                break;
        }
        //convert it now!
    }
	
}

void create_graphics_from_physics_objects(GLInstancingRenderer& renderer)
{
	graphics_from_physics(renderer,false);
}

void sync_graphics_to_physics_objects(GLInstancingRenderer& renderer)
{
	graphics_from_physics(renderer,true);
}



int main(int argc, char* argv[])
{
		
	CommandLineArgs args(argc,argv);

	if (args.CheckCmdLineFlag("help"))
	{
		Usage();
		return 0;
	}

	args.GetCmdLineArgument("enable_interop", useInterop);
	printf("useInterop=%d\n",useInterop);



	args.GetCmdLineArgument("pause_simulation", pauseSimulation);
	printf("pause_simulation=%d\n",pauseSimulation);
	

	
	char* tmpfile = 0;
	args.GetCmdLineArgument("load_bulletfile", tmpfile );
	if (tmpfile)
		fileName = tmpfile;

	printf("load_bulletfile=%s\n",fileName);

	
	printf("\n");
#ifdef __APPLE__
	MacOpenGLWindow* window = new MacOpenGLWindow();
#else
	Win32OpenGLWindow* window = new Win32OpenGLWindow();
#endif
	
	btgWindowConstructionInfo wci(1024,768);
	window->createWindow(wci);
#ifndef __APPLE__
	GLenum err = glewInit();
#endif
    window->runMainLoop();
	window->startRendering();
	window->endRendering();

	int maxObjectCapacity=128*1024;

	GLInstancingRenderer render(maxObjectCapacity);
	render.setCameraDistance(30);
	
		
	render.InitShaders();


//	createSceneProgrammatically(render);
    

	render.writeTransforms();

    window->runMainLoop();

	physics_init();

	physics_create_scene(2);

	create_graphics_from_physics_objects(render);


	while (!window->requestedExit())
	{
		CProfileManager::Reset();
		
		if (shootObject)
		{
			shootObject = false;
			
			btVector3 linVel;// = (m_cameraPosition-m_cameraTargetPosition).normalize()*-100;

			int x,y;
			window->getMouseCoordinates(x,y);
			render.getMouseDirection(&linVel[0],x,y);
			linVel.normalize();
			linVel*=100;

//			btVector3 startPos;
			
			float orn[4] = {0,0,0,1};
			float pos[4];
			render.getCameraPosition(pos);
			
//			demo.setObjectTransform(pos,orn,0);
//			render.writeSingleTransformInstanceToCPU(pos,orn,0);
//			createScene(render, demo);
//			printf("numPhysicsInstances= %d\n", demo.m_numPhysicsInstances);
//			printf("numDynamicPhysicsInstances= %d\n", demo.m_numDynamicPhysicsInstances);
//			render.writeTransforms();
		}


	//	float deltaTime = 1.f/60.f;

        if (!pauseSimulation)
            physics_simulate();
//		stepSimulation(deltaTime);

		{
			BT_PROFILE("sync_graphics_to_physics_objects");
			sync_graphics_to_physics_objects(render);
		}

		{
			BT_PROFILE("render.writeTransforms");
			render.writeTransforms();
		}

		{
			BT_PROFILE("window->startRendering");
			window->startRendering();
		}
		{
			BT_PROFILE("render.RenderScene");
			render.RenderScene();
		}
		{
			BT_PROFILE("window->endRendering");
			window->endRendering();
		}

		{
			BT_PROFILE("glFinish");
			//glFinish();
        //    glFlush();
		}

		CProfileManager::Increment_Frame_Counter();

		static bool printStats  = true;

		
		
		 if (printStats && !pauseSimulation)
		 {
			static int count = 0;
			count--;
			if (count<0)
			{
				count = 100;
				CProfileManager::dumpAll();
				//printStats  = false;
			} else
			{
//				printf(".");
			}
		 }
		

	}

	

	render.CleanupShaders();
	window->closeWindow();
	delete window;
	
	
	
	return 0;
}