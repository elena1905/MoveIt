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

class Ogre3D : public Ogre3DBase
{
public:
    Ogre3D(void);
    virtual ~Ogre3D(void);

protected:
    virtual void createScene(void);
	virtual void createCamera(void);
	virtual void createViewports(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

	/* Skeleton Entities and skeleton SceneNode */
	Ogre::SceneNode* m_pPlayerNode;
	Ogre::Entity* m_pPlayer;

	// Ogre Skeleton and Bones
	Ogre::SkeletonInstance* m_pSkeleton;
	Ogre::Bone* m_pBones;

	/* Kinect Members and Methods */
	bool                    m_bSeatedMode;

    // Current Kinect
    INuiSensor*             m_pNuiSensor;

	Ogre::Vector3            m_Points[NUI_SKELETON_POSITION_COUNT];
	Ogre::Vector3            m_PointsOld[NUI_SKELETON_POSITION_COUNT];

	HANDLE                  m_pSkeletonStreamHandle;
    HANDLE                  m_hNextSkeletonEvent;

	HRESULT                 CreateFirstConnected();
	void                    ProcessSkeleton();
	void                    DrawSkeleton(const NUI_SKELETON_DATA & skel);
	Ogre::Vector3           SkeletonToVector(Vector4 skeletonPoint);
};

 
#endif // #ifndef __Ogre3D_h_