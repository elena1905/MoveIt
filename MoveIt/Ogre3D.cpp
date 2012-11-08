/*
 * Ogre3D.cpp
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#include "Ogre3D.h"
#include "stdafx.h"
#include <strsafe.h>

//-------------------------------------------------------------------------------------
Ogre3D::Ogre3D(void):
	m_hNextSkeletonEvent(INVALID_HANDLE_VALUE),
    m_pSkeletonStreamHandle(INVALID_HANDLE_VALUE),
    m_bSeatedMode(false),
    m_pNuiSensor(NULL)
{

}

//-------------------------------------------------------------------------------------
Ogre3D::~Ogre3D(void)
{
	if (m_pNuiSensor)
    {
        m_pNuiSensor->NuiShutdown();
    }

    if (m_hNextSkeletonEvent && (m_hNextSkeletonEvent != INVALID_HANDLE_VALUE))
    {
        CloseHandle(m_hNextSkeletonEvent);
    }
}

//-------------------------------------------------------------------------------------
void Ogre3D::createScene(void)
{
	/* ========= Kinect: create KinectSensor instance ========= */
	// Instantiate KinectBase
//>>>	m_pKinect = new KinectBase();

	// Create KinectSensor instance for the first connected sensor
	CreateFirstConnected();
	/* ========= End of Kinect: create KinectSensor instance ========= */


	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));

	// Set shadow type
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);


	/* =========== Kinect: create skeleton entities and attach them to skeletonNode ========== */
	m_pHead = mSceneMgr->createEntity("Head", "cube.mesh");
	m_pSkeletonNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
	m_pSkeletonNode->attachObject(m_pHead);
	m_pSkeletonNode->setScale(0.05f, 0.05f, 0.05f);

/*
	m_pShoulderCenter = mSceneMgr->createEntity("ShoulderCenter", "cube.mesh");
	m_pShoulderLeft = mSceneMgr->createEntity("ShoulderLeft", "cube.mesh");
	m_pShoulderRight = mSceneMgr->createEntity("ShoulderRight", "cube.mesh");
	m_pSpine = mSceneMgr->createEntity("Spine", "cube.mesh");
	m_pHipCenter = mSceneMgr->createEntity("HipCenter", "cube.mesh");
	m_pHipLeft = mSceneMgr->createEntity("HipLeft", "cube.mesh");
	m_pHipRight = mSceneMgr->createEntity("HipRight", "cube.mesh");
	m_pKneeLeft = mSceneMgr->createEntity("KneeLeft", "cube.mesh");
	m_pKneeRight = mSceneMgr->createEntity("KneeRight", "cube.mesh");
	m_pAnkleLeft = mSceneMgr->createEntity("AnkleLeft", "cube.mesh");
	m_pAnkleRight = mSceneMgr->createEntity("AnkleRight", "cube.mesh");
*/
	/* =========== Kinect: create skeleton entities and attach them to skeletonNode ========== */


	/* =========== Test: move a cube between two knots ========== */
    // Create an Entity; Cast shadows to it; Attach it to a SceneNode
	mEntity = mSceneMgr->createEntity("Cube", "cube.mesh");
	mEntity->setCastShadows(true);
	mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CubeNode", Ogre::Vector3(0.0f, 10.0f, 25.0f));
	mNode->attachObject(mEntity);
	mNode->setScale(0.05f, 0.05f, 0.05f);
	//mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entCube);
	
	// Create the walking list
    mWalkList.push_back(Ogre::Vector3(550.0f, 10.0f, 50.0f ));
    mWalkList.push_back(Ogre::Vector3(-100.0f, 10.0f, -200.0f));

    // Create objects so we can see movement
    Ogre::Entity *ent;
    Ogre::SceneNode *node;

    ent = mSceneMgr->createEntity("Knot1", "knot.mesh");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Knot1Node", Ogre::Vector3(0.0f, 10.0f, 25.0f));
    node->attachObject(ent);
    node->setScale(0.1f, 0.1f, 0.1f);

    ent = mSceneMgr->createEntity("Knot2", "knot.mesh");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Knot2Node", Ogre::Vector3(550.0f, 10.0f, 50.0f));
    node->attachObject(ent);
    node->setScale(0.1f, 0.1f, 0.1f);

    ent = mSceneMgr->createEntity("Knot3", "knot.mesh");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Knot3Node", Ogre::Vector3(-100.0f, 10.0f, -200.0f));
    node->attachObject(ent);
    node->setScale(0.1f, 0.1f, 0.1f);
	/* =========== End of Test: move a cube between two knots ========== */


	// Create a plane
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

	// Register the plane
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	// Create an Entity from the plane mesh; Display it on the screen
	Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);

	// Set the texture of entGround; Do not cast shadows on it
	entGround->setMaterialName("Examples/Rockwall");
	entGround->setCastShadows(false);

	// Create the light, set its type point and position, set diffuse and specular colour to red
	Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setPosition(Ogre::Vector3(0, 150, 250));
	pointLight->setDiffuseColour(1.0, 0.0, 0.0);
	pointLight->setSpecularColour(1.0, 0.0, 0.0);

	// Create the light, set its type directional and direction, set diffuse and specular colour to yellow
	Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight");
	directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
	directionalLight->setDirection(Ogre::Vector3(0, -1, 1));
	directionalLight->setDiffuseColour(Ogre::ColourValue(0.25, 0.25, 0.0));
	directionalLight->setSpecularColour(Ogre::ColourValue(0.25, 0.25, 0.0));

	// Create the light, set its type spotlight, position, direction and light range,
	// set diffuse and specular colour blue
	Ogre::Light* spotLight = mSceneMgr->createLight("spotLight");
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotLight->setDirection(Ogre::Vector3(-1, -1, 0));
	spotLight->setPosition(Ogre::Vector3(300, 300, 0));
	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
	spotLight->setDiffuseColour(0, 0, 1.0);
	spotLight->setSpecularColour(0, 0, 1.0);
}

//-------------------------------------------------------------------------------------
void Ogre3D::createCamera(void)
{
	// Create the Camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Set the Camera's position and direction
	mCamera->setPosition(Ogre::Vector3(0, 10, 500));
	mCamera->lookAt(Ogre::Vector3(0, 0, 0));

	// Set the near clip distance
	mCamera->setNearClipDistance(5);

	// Test: set the far clip distance
	mSceneMgr->setShadowUseInfiniteFarPlane(false);
	mCamera->setFarClipDistance(1000);

	// create a default camera controller
	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}

//-------------------------------------------------------------------------------------
void Ogre3D::createViewports(void)
{
	// Create one Viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);

	// Set background colour
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}


//-------------------------------------------------------------------------------------
void Ogre3D::createFrameListener(void)
{
    Ogre3DBase::createFrameListener();
    
	/*
	// Set idle animation
    mAnimationState = mEntity->getAnimationState("Idle");
    mAnimationState->setLoop(true);
    mAnimationState->setEnabled(true);
	*/

    // Set default values for variables
    mWalkSpeed = 35.0f;
    mDirection = Ogre::Vector3::ZERO;
}

//-------------------------------------------------------------------------------------
bool Ogre3D::nextLocation(void)
{
    if (mWalkList.empty()) return false;
    mDestination = mWalkList.front();  // this gets the front of the deque
    mWalkList.pop_front();             // this removes the front of the deque
    mDirection = mDestination - mNode->getPosition();
    mDistance = mDirection.normalise();
    return true;
}

//-------------------------------------------------------------------------------------
bool Ogre3D::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	ProcessSkeleton();

	if (mDirection == Ogre::Vector3::ZERO)
	{
		if (nextLocation())
		{
			/*
			// Set walking animation
			mAnimationState = mEntity->getAnimationState("Walk");
			mAnimationState->setLoop(true);
			mAnimationState->setEnabled(true);
			*/
		}//if
	}
	else
	{
		Ogre::Real move = mWalkSpeed * evt.timeSinceLastFrame;
		mDistance -= move;
		if (mDistance <= 0.0f)
		{                 
			mNode->setPosition(mDestination);
			mDirection = Ogre::Vector3::ZERO;				
			// Set animation based on if the robot has another point to walk to. 
			if (!nextLocation())
			{
				/*
				// Set Idle animation                     
				mAnimationState = mEntity->getAnimationState("Idle");
				mAnimationState->setLoop(true);
				mAnimationState->setEnabled(true);
				*/
			}
			else
			{
				// Rotation Code will go here later
				Ogre::Vector3 src = mNode->getOrientation() * Ogre::Vector3::UNIT_X;
				if ((1.0f + src.dotProduct(mDirection)) < 0.0001f)
				{
					mNode->yaw(Ogre::Degree(180));						
				}
				else
				{
					Ogre::Quaternion quat = src.getRotationTo(mDirection);
					mNode->rotate(quat);
				} // else
			}//else
		}
		else
		{
			mNode->translate(mDirection * move);
		} // else
	} // if
	
	/*
	mAnimationState->addTime(evt.timeSinceLastFrame);
	*/

	return Ogre3DBase::frameRenderingQueued(evt);
}



HRESULT Ogre3D::CreateFirstConnected()
{
    INuiSensor * pNuiSensor;

    int iSensorCount = 0;
    HRESULT hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
    {
        return hr;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
        {
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            m_pNuiSensor = pNuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
    }

    if (NULL != m_pNuiSensor)
    {
        // Initialize the Kinect and specify that we'll be using skeleton
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); 
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when skeleton data is available
            m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

            // Open a skeleton stream to receive skeleton data
            hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0); 
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
//>>>        SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}


void Ogre3D::ProcessSkeleton()
{
    NUI_SKELETON_FRAME skeletonFrame = {0};

    HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if ( FAILED(hr) )
    {
        return;
    }

    // smooth out the skeleton data
    m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // Endure Direct2D is ready to draw
    hr = EnsureDirect2DResources( );
    if ( FAILED(hr) )
    {
        return;
    }

    m_pRenderTarget->BeginDraw();
    m_pRenderTarget->Clear( );
    
    RECT rct;
    GetClientRect( GetDlgItem( m_hWnd, IDC_VIDEOVIEW ), &rct);
    int width = rct.right;
    int height = rct.bottom;
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i)
    {
        NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;

        if (NUI_SKELETON_TRACKED == trackingState)
        {
            // We're tracking the skeleton, draw it
            DrawSkeleton(skeletonFrame.SkeletonData[i]);
        }
        else if (NUI_SKELETON_POSITION_ONLY == trackingState)
        {
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
            // we've only received the center point of the skeleton, draw that
            D2D1_ELLIPSE ellipse = D2D1::Ellipse(
                SkeletonToScreen(skeletonFrame.SkeletonData[i].Position, width, height),
                g_JointThickness,
                g_JointThickness
                );

            m_pRenderTarget->DrawEllipse(ellipse, m_pBrushJointTracked);
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
        }
    }

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// Draw a new ramdon circle when old circle touched
	float posX = m_Points[7].x;
	float posY = m_Points[7].y;

	srand((unsigned)time(NULL));
	float randX = 200.0f;
	float randY = 200.0f;

	// Draw a circle
	D2D1_POINT_2F circle = {randX, randY};
	D2D1_ELLIPSE myEllipse = D2D1::Ellipse(circle, 12.0f, 16.0f);
	m_pRenderTarget->DrawEllipse(myEllipse, m_pBrushJointTracked);

	//D2D1_ELLIPSE myEllipse;
	//if ((posX > (randX - 20)) && (posX < (randX + 20)) && (posY > (randY - 20)) && (posY < (randY + 20)))
	if (posX > 180 && posX < 220 && posY > 180 && posY < 220)
	{
		randX = rand() % 300 + 20.0f;
		randY = rand() % 300 + 20.0f;
		D2D1_POINT_2F randCircle = {randX, randY};
		D2D1_ELLIPSE newEllipse = D2D1::Ellipse(randCircle, 12.0f, 16.0f);

		m_pRenderTarget->DrawEllipse(newEllipse, m_pBrushJointTracked);
		
		/*ColeDateTime start_time = ColeDateTime::GetCurrentTime();
		ColeDateTimeSpan end_time = ColeDateTime::GetCurrentTime() - start_time;
		while (end_time.GetTotalSeconds() <= 2)
		{
			m_pRenderTarget->DrawEllipse(myEllipse, m_pBrushJointTracked);
			end_time = ColeDateTime::GetCurrentTime-start_time;
		}*/
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	//m_pRenderTarget->DrawEllipse(myEllipse, m_pBrushJointTracked);

    hr = m_pRenderTarget->EndDraw();

    // Device lost, need to recreate the render target
    // We'll dispose it now and retry drawing
    if (D2DERR_RECREATE_TARGET == hr)
    {
        hr = S_OK;
        DiscardDirect2DResources();
    }
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
}


void Ogre3D::DrawSkeleton(const NUI_SKELETON_DATA & skel)
{      
    int i;

    for (i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
    {
        m_Points[i] = SkeletonToVector(skel.SkeletonPositions[i]);
    }

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // Render Torso
    DrawBone(skel, NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER);
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SPINE);
    DrawBone(skel, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_HIP_CENTER);
    DrawBone(skel, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT);

    // Left Arm
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);

    // Right Arm
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);

    // Left Leg
    DrawBone(skel, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);

    // Right Leg
    DrawBone(skel, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    // Draw the joints in a different color
	if ( skel.eSkeletonPositionTrackingState[0] == NUI_SKELETON_POSITION_INFERRED )
    {
		m_pSkeletonNode->setPosition(m_Points[0]);
    }
	else if ( skel.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_TRACKED )
    {
		m_pSkeletonNode->setPosition(m_Points[0]);
    }
/*
    for (i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
    {
//>>>        D2D1_ELLIPSE ellipse = D2D1::Ellipse( m_Points[i], g_JointThickness, g_JointThickness );

        if ( skel.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_INFERRED )
        {
//>>>            m_pRenderTarget->DrawEllipse(ellipse, m_pBrushJointInferred);
			m_pSkeletonNode->setPostition(m_Points[i]);
        }
        else if ( skel.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_TRACKED )
        {
//>>>            m_pRenderTarget->DrawEllipse(ellipse, m_pBrushJointTracked);
        }
    }
*/
}


Ogre::Vector3 Ogre3D::SkeletonToVector(Vector4 skeletonPoint)
{
    FLOAT x, y;

    // Calculate the skeleton's position on the screen
    // NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
    NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y);

    Ogre::Vector3 position = Ogre::Vector3(x+10, y+10, -50.0f);

    return position;
}
