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

	static Ogre::Math Math;

	static Ogre::Radian TWOPI;
	static Ogre::Radian PI;
	static Ogre::Radian PIOVER2;

	static Ogre::Vector3 Basis;
	static Ogre::Quaternion PiQuaternion;
};
