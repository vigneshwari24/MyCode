#include <sensorsapi.h>
#include <InitGuid.h>
#include <Sensors.h>
#include <conio.h>
//#include "Orientaion.h"
#define WM_LANDSCAPE			WM_USER + 2 
#define WM_PORTRAIT				WM_USER + 3
#define WM_LANDSCAPE180			WM_USER + 4 
#define WM_PORTRAIT180			WM_USER + 5
#define LAYOUT_LANDSCAPE			1
#define LAYOUT_PORTRAIT				2
#define LAYOUT_LANDSCAPE180			3
#define LAYOUT_PORTRAIT180			4

#ifdef  ACCELEROMETERDLL_EXPORTS
#define ACCELEROMETERDLL __declspec(dllexport)
#else
#define ACCELEROMETERDLL __declspec(dllimport)
#endif


class ACCELEROMETERDLL SensorEvents : public ISensorEvents
{
	public:
	SensorEvents();
	SensorEvents(HWND hWnd);
	virtual ~SensorEvents();
	
	STDMETHOD(QueryInterface)(REFIID riid, void** ppObject );
    ULONG _stdcall AddRef();
    ULONG _stdcall Release();

	STDMETHOD(OnStateChanged)(ISensor* pSensor, SensorState state);
    STDMETHOD(OnDataUpdated)(ISensor* pSensor, ISensorDataReport* pNewData);
    STDMETHOD(OnEvent)(ISensor* pSensor, REFGUID eventID, IPortableDeviceValues* pEventData);
    STDMETHOD(OnLeave)(REFSENSOR_ID sensorID);
	HRESULT GetSensorData(ISensor* pSensor);
	HRESULT GetSensorData(ISensor *pSensor, ISensorDataReport *pDataReport);
	
private:
	HWND           m_hWnd;
	

};

class ACCELEROMETERDLL  Accelerometer 
{
	public:
	Accelerometer();
	~Accelerometer();
	bool FindDevice(GUID DeviceCategory,HWND hWnd);
	bool InitSensorEvent();
	
	private:
	SensorEvents *              m_pSensorEventsClass;
	ISensorManager* 			m_pSensorManager;
	ISensorCollection*			m_pSensorCollection;
	ISensorEvents *				m_pSensorEvents;
	HWND						m_hWnd;

};

