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

#include "MathHelper.h"
#include "Player.h"
#include "KinectHelper.h"

#include <array>
#include <queue>

#include "stdafx.h"
#include "Ogre3DBase.h"
#include "resource.h"


class Ogre3D : public Ogre3DBase
{
public:
    Ogre3D(void);
    virtual ~Ogre3D(void);

	MathHelper* m_pMath;
	KinectHelper* m_pKinectHelper;
	//Player* m_pModel;
	Player* m_pPlayer;

	float mTimeElapsed;

protected:
    virtual void createScene(void);
	virtual void createCamera(void);
	virtual void createViewports(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

	Ogre::Viewport* m_pViewport;
};

 
#endif // #ifndef __Ogre3D_h_