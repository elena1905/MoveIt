/*
 * MathHelper.h
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#pragma once


#include "stdafx.h"
#include "Ogre3DBase.h"


class MathHelper
{
public:
    MathHelper();
    ~MathHelper();

	Ogre::Vector3 Basis;
	Ogre::Quaternion PiQuaternion;
	Ogre::Math Math;
	Ogre::Radian TWOPI;
	Ogre::Radian PI;
	Ogre::Radian PIOVER2;
};
