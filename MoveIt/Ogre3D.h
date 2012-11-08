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
	
	virtual void createFrameListener(void); 
    virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
    virtual bool nextLocation(void);
	
//>>>	KinectBase* m_pKinect;				   // Kinect instance

	Ogre::Real mDistance;                  // The distance the object has left to travel
    Ogre::Vector3 mDirection;              // The direction the object is moving
    Ogre::Vector3 mDestination;            // The destination the object is moving towards
 
    Ogre::AnimationState *mAnimationState; // The current animation state of the object
 
    Ogre::Entity *mEntity;                 // The Entity we are animating
    Ogre::SceneNode *mNode;                // The -SceneNode that the Entity is attached to
    std::deque<Ogre::Vector3> mWalkList;   // The list of points we are walking to
 
    Ogre::Real mWalkSpeed;                 // The speed at which the object is moving

	/* Skeleton Entities and skeleton SceneNode */
	Ogre::SceneNode* m_pSkeletonNode;

	Ogre::Entity* m_pHead;

	Ogre::Entity* m_pShoulderCenter;
	Ogre::Entity* m_pShoulderLeft;
	Ogre::Entity* m_pShoulderRight;

	Ogre::Entity* m_pSpine;

	Ogre::Entity* m_pHipCenter;
	Ogre::Entity* m_pHipLeft;
	Ogre::Entity* m_pHipRight;

	Ogre::Entity* m_pKneeLeft;
	Ogre::Entity* m_pKneeRight;

	Ogre::Entity* m_pAnkleLeft;
	Ogre::Entity* m_pAnkleRight;

	/* Kinect Members and Methods */
	bool                    m_bSeatedMode;

    // Current Kinect
    INuiSensor*             m_pNuiSensor;

	Ogre::Vector3            m_Points[NUI_SKELETON_POSITION_COUNT];

	HANDLE                  m_pSkeletonStreamHandle;
    HANDLE                  m_hNextSkeletonEvent;

	HRESULT                 CreateFirstConnected();
	void                    ProcessSkeleton();
	void                    DrawSkeleton(const NUI_SKELETON_DATA & skel);
	Ogre::Vector3           SkeletonToVector(Vector4 skeletonPoint);
};

 
#endif // #ifndef __Ogre3D_h_