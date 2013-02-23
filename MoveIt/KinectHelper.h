/*
 * KinectHelper.h
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#pragma once


#include <array>

#include "stdafx.h"
#include "Ogre3DBase.h"
#include "NuiApi.h"
#include "MathHelper.h"


class KinectHelper
{
    static const int        cScreenWidth  = 320;
    static const int        cScreenHeight = 240;
	static const int		ARR_SIZE = 8;

public:
    KinectHelper();
    ~KinectHelper();

	HRESULT                 CreateFirstConnected();
	void                    ProcessSkeleton(Ogre::SceneNode* pNode, 
											std::array<Ogre::Bone*, 8>& rBoneArray);
	void                    RotateBones(const NUI_SKELETON_DATA & skel,
										Ogre::SceneNode* pNode,
										std::array<Ogre::Bone*, 8>& rBoneArray);
	Ogre::Vector3           SkeletonToVector3(Vector4 skeletonPoint);

private:
	/* Kinect Members and Methods */
	bool                    m_bSeatedMode;

	// Current Kinect
	INuiSensor*             m_pNuiSensor;

	Ogre::Vector3           m_Points[NUI_SKELETON_POSITION_COUNT];

	HANDLE                  m_pSkeletonStreamHandle;
	HANDLE                  m_hNextSkeletonEvent;

	MathHelper*				m_pMath;
};
