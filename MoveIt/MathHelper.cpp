/*
 * MathHelper.cpp
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#include "MathHelper.h"


//-----------------------------------------------------------
Ogre::Radian MathHelper::TWOPI = Ogre::Radian(Math.TWO_PI);
Ogre::Radian MathHelper::PI = Ogre::Radian(Math.PI);
Ogre::Radian MathHelper::PIOVER2 = Ogre::Radian(Math.HALF_PI);

Ogre::Vector3 MathHelper::Basis = Ogre::Vector3(0.0f, -1.0f, 0.0f);
Ogre::Quaternion MathHelper::PiQuaternion = Ogre::Quaternion(PI, Ogre::Vector3(0, 1, 0));


//-----------------------------------------------------------
MathHelper::MathHelper()
{
	
}


//-----------------------------------------------------------
MathHelper::~MathHelper()
{
    
}
