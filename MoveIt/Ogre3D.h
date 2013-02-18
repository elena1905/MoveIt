/*
 * Ogre3D.h
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#ifndef __Ogre3D_h_
#define __Ogre3D_h_
 
#include "Ogre3DBase.h"
#include "resource.h"
#include "NuiApi.h"
//#include "KinectBase.h"

// Include msxml6.dll from stdafx.h to make use of DOM
#include "stdafx.h"

#include <array>
#include <queue>


class Ogre3D : public Ogre3DBase
{
	static const int        cScreenWidth  = 320;
	static const int        cScreenHeight = 240;

public:
    Ogre3D(void);
    virtual ~Ogre3D(void);

protected:
    virtual void createScene(void);
	virtual void createCamera(void);
	virtual void createViewports(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

	Ogre::Vector3 m_Basis;
	Ogre::Quaternion m_QuatPI;
	Ogre::Math m_Math;
	Ogre::Radian TWOPI;
	Ogre::Radian PI;
	Ogre::Radian PIOVER2;

	Ogre::Viewport* m_pViewport;

	/* Skeleton Entities and skeleton SceneNode */
	Ogre::SceneNode* m_pPlayerNode;
	Ogre::Entity* m_pPlayer;
	Ogre::SceneNode* m_pPlayerNode2;
	Ogre::Entity* m_pPlayer2;

	// Ogre Skeleton and Bones
	static const int ARR_SIZE = 8;
	std::array<Ogre::Bone*, ARR_SIZE> m_BoneArray;
	Ogre::SkeletonInstance* m_pSkeleton;
	std::array<Ogre::Bone*, ARR_SIZE> m_BoneArray2;
	Ogre::SkeletonInstance* m_pSkeleton2;

	// Manual curved line
	static const int QUEUE_SIZE = 40;
	std::queue<Ogre::Vector3> m_PointQueL;
	std::queue<Ogre::Vector3> m_PointQueR;
	Ogre::SceneNode* m_pLineNode;
	Ogre::ManualObject* m_pLineL;
	Ogre::ManualObject* m_pLineR;

	/* Kinect Members and Methods */
	bool                    m_bSeatedMode;

    // Current Kinect
    INuiSensor*             m_pNuiSensor;

	Ogre::Vector3           m_Points[NUI_SKELETON_POSITION_COUNT];

	HANDLE                  m_pSkeletonStreamHandle;
    HANDLE                  m_hNextSkeletonEvent;

	HRESULT                 CreateFirstConnected();
	void                    ProcessSkeleton();
	void                    RotateBones(const NUI_SKELETON_DATA & skel);
	Ogre::Vector3           SkeletonToVector3(Vector4 skeletonPoint);

	// DOM XML Parser
	IXMLDOMDocumentPtr m_pDomDocument;
	IXMLDOMElementPtr m_pDomRoot;
};

 
#endif // #ifndef __Ogre3D_h_