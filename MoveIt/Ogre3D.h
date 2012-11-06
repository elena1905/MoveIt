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
	

	Ogre::Real mDistance;                  // The distance the object has left to travel
    Ogre::Vector3 mDirection;              // The direction the object is moving
    Ogre::Vector3 mDestination;            // The destination the object is moving towards
 
    Ogre::AnimationState *mAnimationState; // The current animation state of the object
 
    Ogre::Entity *mEntity;                 // The Entity we are animating
    Ogre::SceneNode *mNode;                // The -SceneNode that the Entity is attached to
    std::deque<Ogre::Vector3> mWalkList;   // The list of points we are walking to
 
    Ogre::Real mWalkSpeed;                 // The speed at which the object is moving
};

 
#endif // #ifndef __Ogre3D_h_