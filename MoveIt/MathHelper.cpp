/*
 * MathHelper.cpp
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#include "MathHelper.h"


MathHelper::MathHelper()
{
	TWOPI = Ogre::Radian(Math.TWO_PI);
	PI = Ogre::Radian(Math.PI);
	PIOVER2 = Ogre::Radian(Math.HALF_PI);

	Basis = Ogre::Vector3(0.0f, -1.0f, 0.0f);

	PiQuaternion.FromAngleAxis(PI, Ogre::Vector3(0, 1, 0));
}


MathHelper::~MathHelper()
{
    
}


//-------------------------------------------------------------------------------------
