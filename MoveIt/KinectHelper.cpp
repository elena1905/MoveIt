/*
 * KinectHelper.cpp
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#include "KinectHelper.h"


KinectHelper::KinectHelper() :
    m_hNextSkeletonEvent(INVALID_HANDLE_VALUE),
    m_pSkeletonStreamHandle(INVALID_HANDLE_VALUE),
    m_bSeatedMode(false),
    m_pNuiSensor(NULL)
{

}


KinectHelper::~KinectHelper()
{
    if (m_pNuiSensor)
    {
        m_pNuiSensor->NuiShutdown();
    }

    if (m_hNextSkeletonEvent && (m_hNextSkeletonEvent != INVALID_HANDLE_VALUE))
    {
        CloseHandle(m_hNextSkeletonEvent);
    }

    SafeRelease(m_pNuiSensor);
}


HRESULT KinectHelper::CreateFirstConnected()
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


void KinectHelper::ProcessSkeleton(void)
{
	NUI_SKELETON_FRAME skeletonFrame = {0};

	HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
	if ( FAILED(hr) )
	{
		return;
	}

	// smooth out the skeleton data
	m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);

	ClearQuaternionQueue();
	ClearCentralPosQueue();

	// Process multiple skeletons in a skeleton frame
	for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i)
	{
		NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;

		if (NUI_SKELETON_TRACKED == trackingState)
		{
			// We're tracking the skeleton, rotate the bones
			RotateBones(skeletonFrame.SkeletonData[i]);
		}
		else if (NUI_SKELETON_POSITION_ONLY == trackingState)
		{
			// Rotate the bones
			RotateBones(skeletonFrame.SkeletonData[i]);
		}
	}
}


//-------------------------------------------------------------------------------------
void KinectHelper::RotateBones(const NUI_SKELETON_DATA & skel)
{
	// Convert skeleton positions to vector3 format
	// Process multiple joints in a skeleton
	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
	{
		m_Points[i] = SkeletonToVector3(skel.SkeletonPositions[i]);
	}

	// Get the central point: HIP_CENTER
	m_CentralX = m_Points[NUI_SKELETON_POSITION_HIP_CENTER].x;
	//	m_CentralY = m_Points[NUI_SKELETON_POSITION_SHOULDER_CENTER].y;

	// Map Kinect coordinate to Ogre coordinate
	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
	{
		MapKinectToOgre(m_Points[i]);
	}

	// Get the player's central position
	m_CentralZ = -m_Points[NUI_SKELETON_POSITION_HIP_CENTER].z;
	Ogre::Vector3 centralPosition = Ogre::Vector3(m_CentralX, 0.0f, m_CentralZ);
	m_CentralPosQueue.push(centralPosition);

	// Calculate bone rotation vectors
	Ogre::Vector3* bonesRot = new Ogre::Vector3[ARR_SIZE];
	bonesRot[0] = m_Points[NUI_SKELETON_POSITION_ELBOW_LEFT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT];
	bonesRot[1] = m_Points[NUI_SKELETON_POSITION_WRIST_LEFT] - m_Points[NUI_SKELETON_POSITION_ELBOW_LEFT];
	bonesRot[2] = m_Points[NUI_SKELETON_POSITION_ELBOW_RIGHT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
	bonesRot[3] = m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT] - m_Points[NUI_SKELETON_POSITION_ELBOW_RIGHT];
	bonesRot[4] = m_Points[NUI_SKELETON_POSITION_KNEE_LEFT] - m_Points[NUI_SKELETON_POSITION_HIP_LEFT];
	bonesRot[5] = m_Points[NUI_SKELETON_POSITION_ANKLE_LEFT] - m_Points[NUI_SKELETON_POSITION_KNEE_LEFT];
	bonesRot[6] = m_Points[NUI_SKELETON_POSITION_KNEE_RIGHT] - m_Points[NUI_SKELETON_POSITION_HIP_RIGHT];
	bonesRot[7] = m_Points[NUI_SKELETON_POSITION_ANKLE_RIGHT] - m_Points[NUI_SKELETON_POSITION_KNEE_RIGHT];

	Ogre::Quaternion q;
	Ogre::Vector3 axis;
	Ogre::Radian angle;

	// Rotate bones
	if ( skel.eSkeletonPositionTrackingState[0] == NUI_SKELETON_POSITION_INFERRED )
	{

	}
	else if ( skel.eSkeletonPositionTrackingState[0] == NUI_SKELETON_POSITION_TRACKED )
	{
		// 
		for(int i = 0; i < ARR_SIZE; i++)
		{
			bonesRot[i].normalise();

			axis = bonesRot[i].crossProduct(MathHelper::Basis);
			angle = -MathHelper::Math.ACos(bonesRot[i].dotProduct(MathHelper::Basis));

			q.FromAngleAxis(angle, axis);

			m_QuaternionQueue[i].push(q);
		}
	}

	delete [] bonesRot;
}

//-------------------------------------------------------------------------------------
Ogre::Vector3 KinectHelper::SkeletonToVector3(Vector4 skeletonPoint)
{
	LONG x, y;
	USHORT depth;

	// Calculate the skeleton's position on the screen
	// NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
	NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y, &depth);

	float posX = static_cast<float>(x);
	float posY = static_cast<float>(y);
	float posZ = static_cast<float>(depth); // Unit of depth value is millimeter

	Ogre::Vector3 position = Ogre::Vector3(posX, posY, posZ);

	return position;
}


//-------------------------------------------------------------------------------------
void KinectHelper::MapKinectToOgre(Ogre::Vector3& kinectPoint)
{
	// Map Kinect coordinate to Ogre coordinate
	kinectPoint.x = 2 * m_CentralX - kinectPoint.x;
	kinectPoint.y = 220.0f - kinectPoint.y;
	//		kinectPoint.y = 2 * m_CentralY - kinectPoint.y;
	kinectPoint.z = kinectPoint.z / 100;
}


//-------------------------------------------------------------------------------------
void KinectHelper::ClearQuaternionQueue()
{
	for (int i = 0; i < ARR_SIZE; i++)
	{
		m_QuaternionQueue[i].swap(std::queue<Ogre::Quaternion>());
	}
}


//-------------------------------------------------------------------------------------
void KinectHelper::ClearCentralPosQueue()
{
	m_CentralPosQueue.swap(std::queue<Ogre::Vector3>());
}