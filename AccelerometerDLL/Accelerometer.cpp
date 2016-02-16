#include "stdafx.h"
#include "Accelerometer.h"
#include <math.h>


ISensor* g_pSensor = NULL;
int g_OrientationFlag = 0;
SensorEvents::SensorEvents()
{
	
}
SensorEvents::SensorEvents(HWND hWnd)
{
	m_hWnd = hWnd;
}
SensorEvents::~SensorEvents()
{

}
STDMETHODIMP SensorEvents::QueryInterface(REFIID riid, void** ppObject)
{
    HRESULT hr = S_OK;

    *ppObject = NULL;
    if (riid == __uuidof(ISensorEvents))
    {
        *ppObject = reinterpret_cast<ISensorEvents*>(this);
    }
    else if (riid == IID_IUnknown)
    {
        *ppObject = reinterpret_cast<IUnknown*>(this);
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    if (SUCCEEDED(hr))
    {
        (reinterpret_cast<IUnknown*>(*ppObject))->AddRef();
    }

    return hr;
}


ULONG _stdcall SensorEvents::AddRef()
{
	OutputDebugString(L" \nAddRef");
     return S_OK;
}


ULONG _stdcall SensorEvents::Release()
{
	OutputDebugString(L" \n Release");   
	return S_OK;
}


STDMETHODIMP SensorEvents::OnStateChanged(ISensor *pSensor, SensorState state)
{
    OutputDebugString(L" \n State Changed");
	
	return S_OK;
}


STDMETHODIMP SensorEvents::OnDataUpdated(ISensor *pSensor, ISensorDataReport *pNewData)
{
	GetSensorData(pSensor,pNewData);
	return S_OK;
}


STDMETHODIMP SensorEvents::OnEvent(ISensor* /*pSensor*/, REFGUID /*eventID*/, IPortableDeviceValues* /*pEventData*/)
{
   OutputDebugString(L" \n OnEvent");
   GetSensorData(g_pSensor);
   return S_OK;
}


STDMETHODIMP SensorEvents::OnLeave(REFSENSOR_ID sensorID)
{
	OutputDebugString(L" \n OnLeave");
	return S_OK;
}
HRESULT SensorEvents::GetSensorData(ISensor* pSensor)
{
   
	return S_OK;
}


HRESULT SensorEvents::GetSensorData(ISensor *pSensor, ISensorDataReport *pDataReport)
{
	HRESULT hr = S_OK;
	if (NULL != pSensor && NULL != pDataReport)
    {
		//OutputDebugString(L"\n inside");
		if (SUCCEEDED(hr))
        {
			
			PROPVARIANT pv1,pv2,pv3;
            PropVariantInit(&pv1);
			PropVariantInit(&pv2);
			PropVariantInit(&pv3);
			
			hr = pDataReport->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_X_G, &pv1);
			hr = pDataReport->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Y_G, &pv2);
			hr = pDataReport->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Z_G, &pv3);
        
			if (SUCCEEDED(hr))
            {
			//Partial Landscape and portrait oritentaiton
				if(pv2.fltVal < -0.9)
				{
				//	OutputDebugString(L"\nLandScape");
					if(g_OrientationFlag != LAYOUT_LANDSCAPE)
					{
						g_OrientationFlag = LAYOUT_LANDSCAPE;
						PostMessage(m_hWnd,WM_LANDSCAPE,NULL,NULL);
					}
				}else if(pv1.fltVal > 0.9)
				{
					//OutputDebugString(L"\nPortrait");
					if(g_OrientationFlag != LAYOUT_PORTRAIT)
					{
						g_OrientationFlag = LAYOUT_PORTRAIT;
						PostMessage(m_hWnd,WM_PORTRAIT,NULL,NULL);
					}
				}else if(pv2.fltVal > 0.9)
				{
					//OutputDebugString(L"\n Landscape 180");
					if(g_OrientationFlag != LAYOUT_LANDSCAPE180)
					{
						g_OrientationFlag = LAYOUT_LANDSCAPE180;
						PostMessage(m_hWnd,WM_LANDSCAPE180,NULL,NULL);
					}
				}else if(pv1.fltVal < -0.9)
				{
					//OutputDebugString(L"\n Portrait  180");
					if(g_OrientationFlag != LAYOUT_PORTRAIT180)
					{
						g_OrientationFlag = LAYOUT_PORTRAIT180;
						PostMessage(m_hWnd,WM_PORTRAIT180,NULL,NULL);
					}
				}
			}
            PropVariantClear(&pv1);
			PropVariantClear(&pv2);
			PropVariantClear(&pv3);
        }
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;

    return S_OK;
}
Accelerometer::Accelerometer()
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	
	
}
Accelerometer::~Accelerometer()
{
	CoUninitialize();
}
bool Accelerometer::FindDevice(GUID DeviceCategory,HWND hWnd)
{
	m_hWnd = hWnd;
	HRESULT hr = ::CoCreateInstance(CLSID_SensorManager, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&m_pSensorManager));
	if (FAILED(hr))
	{
		return false;
	}
	hr =m_pSensorManager->GetSensorsByType((const GUID)DeviceCategory, &m_pSensorCollection);
	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		ULONG ulCount = 0;
        hr = m_pSensorCollection->GetCount(&ulCount);
		OutputDebugString(L"\n Number of Devices  \n");
		if (SUCCEEDED(hr))
        {
			for(ULONG i=0; i < ulCount; i++)
			{
				hr = m_pSensorCollection->GetAt(i, &g_pSensor);
				if(SUCCEEDED(hr))
				{ 
					VARIANT_BOOL bSupported = VARIANT_FALSE;
					hr = g_pSensor->SupportsDataField(SENSOR_DATA_TYPE_TILT_X_DEGREES, &bSupported);
					if(bSupported == VARIANT_TRUE)
						OutputDebugString(L"\n TILT X DEGREES \n ");
					bSupported = VARIANT_FALSE;
					hr = g_pSensor->SupportsDataField(SENSOR_DATA_TYPE_ACCELERATION_X_G, &bSupported);
					if(bSupported == VARIANT_TRUE)
						OutputDebugString(L"\n SENSOR_DATA_TYPE_ACCELERATION_X_G \n ");
					bSupported = VARIANT_FALSE;
					hr = g_pSensor->SupportsDataField(SENSOR_DATA_TYPE_MAGNETIC_HEADING_DEGREES, &bSupported);
					if(bSupported == VARIANT_TRUE)
					{
						OutputDebugString(L"\n  SENSOR_DATA_TYPE_MAGNETIC_HEADING_DEGREES \n ");
					}
					bSupported = VARIANT_FALSE;
					hr = g_pSensor->SupportsDataField(SENSOR_DATA_TYPE_LATITUDE_DEGREES, &bSupported);
					if(bSupported == VARIANT_TRUE)
					{
						OutputDebugString(L"\n SENSOR_DATA_TYPE_LATITUDE_DEGREES \n ");
											
					}
				}
			
				OutputDebugString(L"\n read  sensor \n ");
			}
        }
	}

	return true;
}
bool Accelerometer::InitSensorEvent()
{
	m_pSensorEventsClass = new SensorEvents(m_hWnd);  
	HRESULT hr = m_pSensorEventsClass->QueryInterface(IID_PPV_ARGS(&m_pSensorEvents)); 
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Cannot query ISensorEvents interface for our callback class.",L"Sensor C++ Sample", MB_OK | MB_ICONERROR);
		return false;
	}	

	hr = g_pSensor->SetEventSink(m_pSensorEvents); 
	if (FAILED(hr))
	{
	    ::MessageBox(NULL, L"Cannot SetEventSink on the Sensor to our callback class.",L"Sensor C++ Sample", MB_OK | MB_ICONERROR);
		
		return false;
	}
	if(SUCCEEDED(hr))
		OutputDebugString(L"\n Succeeded");
	return true;
}


