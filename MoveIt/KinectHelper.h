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
#include <queue>

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
	
	void                    ProcessSkeleton(void);
	void                    RotateBones(const NUI_SKELETON_DATA & skel);

	std::queue<Ogre::Vector3> m_CentralPosQueue;
	std::queue<Ogre::Quaternion> m_QuaternionQueue[ARR_SIZE];

private:
	Ogre::Vector3           SkeletonToVector3(Vector4 skeletonPoint);
	void					MapKinectToOgre(Ogre::Vector3& kinectPoint);
	void					ClearQuaternionQueue();
	void					ClearCentralPosQueue();

	Ogre::Real				m_CentralX, m_CentralY, m_CentralZ;

	/* Kinect Members and Methods */
	bool                    m_bSeatedMode;

	// Current Kinect
	INuiSensor*             m_pNuiSensor;

	Ogre::Vector3           m_Points[NUI_SKELETON_POSITION_COUNT];

	HANDLE                  m_pSkeletonStreamHandle;
	HANDLE                  m_hNextSkeletonEvent;
};
