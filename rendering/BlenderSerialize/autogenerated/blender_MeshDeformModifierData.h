/* Copyright (C) 2006 Charlie C
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/
// Auto generated from makesdna dna.c
#ifndef __BLENDER_MESHDEFORMMODIFIERDATA__H__
#define __BLENDER_MESHDEFORMMODIFIERDATA__H__


// -------------------------------------------------- //
#include "blender_Common.h"
#include "blender_ModifierData.h"

namespace Blender {


    // ---------------------------------------------- //
    class MeshDeformModifierData
    {
    public:
        ModifierData modifier;
        Object *object;
        char defgrp_name[32];
        short gridsize;
        short needbind;
        short flag;
        short pad;
        float *bindweights;
        float *bindcos;
        int totvert;
        int totcagevert;
        MDefCell *dyngrid;
        MDefInfluence *dyninfluences;
        int *dynverts;
        int *pad2;
        int dyngridsize;
        int totinfluence;
        vec3f dyncellmin;
        float dyncellwidth;
        float bindmat[4][4];
    };
}


#endif//__BLENDER_MESHDEFORMMODIFIERDATA__H__
