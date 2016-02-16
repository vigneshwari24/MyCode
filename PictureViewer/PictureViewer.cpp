//source

#ifndef WINVER                  // Allow use of features specific to Windows XP or later.
#define WINVER 0x0501           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410   // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE               // Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600        // Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <windows.h>
#include <wincodec.h>
#include <commdlg.h>
#include <commctrl.h>
#include<winbase.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include "PictureViewer.h"

template <typename T>
inline void SafeRelease(T *&p)
{
    if (NULL != p)
    {
        p->Release();
        p = NULL;
    }
}


/******************************************************************
*  Application entrypoint                                         *
******************************************************************/

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR pszCmdLine,
    int nCmdShow)
{
	
	UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pszCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	
    if (SUCCEEDED(hr))
    {
        {
            PicModifier app;
            HWND hWndMain = NULL;

            hWndMain = app.Initialize(hInstance);
            hr = hWndMain ? S_OK : E_FAIL;

            if (SUCCEEDED(hr))
            {
                BOOL fRet;
                MSG msg;

                // Load accelerator table
                HACCEL haccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_ACCEL_TABLE));
                if (haccel == NULL) 
                {
                    hr = E_FAIL;
                }
                // Main message loop:
                while ((fRet = GetMessage(&msg, NULL, 0, 0)) != 0)
                {
                    if (fRet == -1)
                    {
                        break;
                    }
                    else
                    {
                        if (!TranslateAccelerator(hWndMain, haccel, &msg))
                        {
                            TranslateMessage(&msg);
                            DispatchMessage(&msg);
                        }
                    }
                }
            }
        }
        CoUninitialize();
    }

    return 0;
}

/******************************************************************
*  Initialize member data                                         *
******************************************************************/

PicModifier::PicModifier()
    :
    m_pD2DBitmap(NULL),
    m_pSourceFrame(NULL),
    m_pConvertedSourceBitmap(NULL),
    m_pIWICFactory(NULL),
    m_pD2DFactory(NULL),
    m_pRT(NULL),
    m_uCurrentLevel(0)
 
{
	m_flag=0;
	m_height=0;
	m_width=0;
	m_dispf=0;

}

/******************************************************************
*  Tear down resources                                            *
******************************************************************/

PicModifier::~PicModifier()
{
    SafeRelease(m_pD2DBitmap);
    SafeRelease(m_pSourceFrame);
    SafeRelease(m_pConvertedSourceBitmap);
    SafeRelease(m_pIWICFactory);
    SafeRelease(m_pD2DFactory);
    SafeRelease(m_pRT);
}

/******************************************************************
*  Create application window and resources                        *
******************************************************************/

HWND PicModifier::Initialize(HINSTANCE hInstance)
{
    m_winx=(float)::GetSystemMetrics(SM_CXSCREEN);
	m_winy=(float)::GetSystemMetrics(SM_CYSCREEN);

	HRESULT hr = S_OK;

    // Create WIC factory
    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pIWICFactory)
        );

    if (SUCCEEDED(hr))
    {
        // Create D2D factory
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
    }

    if (SUCCEEDED(hr))
    {
        WNDCLASSEX wcex;

        // Register window class
        wcex.cbSize        = sizeof(WNDCLASSEX);
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = PicModifier::s_WndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = hInstance;
        wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
        wcex.lpszClassName = L"PictureViewer";
        wcex.hIconSm       = NULL;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
        m_hInst = hInstance;

        hr = RegisterClassEx(&wcex) ? S_OK : E_FAIL;
    }

    HWND hWnd = NULL;
    if (SUCCEEDED(hr))
    {
        // Create main window
		
        hWnd = CreateWindow(
            L"PictureViewer",
            L"PictureViewer",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE ,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
           (int) m_winx,
           (int) m_winy,
            NULL,
            NULL,
            hInstance,
            this
            );
    }
	RECT rectWorkArea;
	::GetClientRect(hWnd,&rectWorkArea);
	//SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);
	m_winx=(float)(rectWorkArea.right-rectWorkArea.left);
	m_winy=(float)(rectWorkArea.bottom-rectWorkArea.top);
	
    return hWnd;
}

/**********************************************************************************
*  Load an image file and create the frame that has the IWICProgressiveLeveLCtrl  *
**********************************************************************************/

HRESULT PicModifier::CreateProgressiveCtrlFromFile(HWND hWnd)
{
    HRESULT hr = S_OK;

    WCHAR szFileName[MAX_PATH];
	m_flag=1;
    // Create the open dialog box and locate the image file
    if (LocateImageFile(hWnd, szFileName, ARRAYSIZE(szFileName)))
    {
        // Create a decoder
        IWICBitmapDecoder *pDecoder = NULL;

        hr = m_pIWICFactory->CreateDecoderFromFilename(
            szFileName,                      // Image to be decoded
            NULL,                            // Do not prefer a particular vendor
            GENERIC_READ,                    // Desired read access to the file
            WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
            &pDecoder                        // Pointer to the decoder
            );

        // Retrieve the first frame of the image from the decoder
        if (SUCCEEDED(hr))
        {
            // Need to release the previously source bitmap.
            // For each new image file, we need to create a new source bitmap
            SafeRelease(m_pSourceFrame);
			hr = pDecoder->GetFrame(0, &m_pSourceFrame);
			UINT w,h;
			m_pSourceFrame->GetSize(&w,&h);
			m_aspectratio=(float)w/(float)h;
			m_picW=m_width=(float)w;
			m_picH=m_height=(float)h;
		 }

        IWICProgressiveLevelControl *pProgressive = NULL;

        if (SUCCEEDED(hr))
        {
            // Query for Progressive Level Control Interface
            hr = m_pSourceFrame->QueryInterface(IID_PPV_ARGS(&pProgressive));

            // Update progressive level count and reset the current level
            if (SUCCEEDED(hr))
            {
                m_uCurrentLevel = 0;
                hr = pProgressive->GetLevelCount(&m_uLevelCount);
            }
            else
            {
                MessageBox(hWnd, L"Image has no progressive encoding, select another one.", L"Application Error", 
                    MB_ICONEXCLAMATION | MB_OK);
            }
        }
        else
        {
            MessageBox(hWnd, L"Failed to load image, select another one.", L"Application Error", 
                MB_ICONEXCLAMATION | MB_OK);
        }

        SafeRelease(pDecoder);
        SafeRelease(pProgressive);
    }

    return hr;
}

/******************************************************************
*  Create a D2DBitmap at a specific progressive level             *
******************************************************************/
HRESULT PicModifier::CreateD2DBitmapFromProgressiveCtrl(HWND hWnd, UINT uLevel)
{
    HRESULT hr = E_FAIL;
    
    // Make sure source frame has been decoded successfully
    if (m_pSourceFrame)
    {
        // Query for Progressive Interface Control
       IWICProgressiveLevelControl *pProgressive = NULL;
        
        hr = m_pSourceFrame->QueryInterface(IID_PPV_ARGS(&pProgressive));

        // Set the progessive level before converting the format
        if (SUCCEEDED(hr))
        {
            hr = pProgressive->SetCurrentLevel(uLevel);
        }

        if (SUCCEEDED(hr))
        {
            SafeRelease(m_pConvertedSourceBitmap);
            hr = m_pIWICFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
        }

        if (SUCCEEDED(hr) && m_pConvertedSourceBitmap)
        {
            // Format convert the current progressive level frame to 32bppPBGRA 
            hr = m_pConvertedSourceBitmap->Initialize(
                m_pSourceFrame,                  // Source bitmap to convert
                GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
                WICBitmapDitherTypeNone,         // Specified dither pattern
                NULL,                            // Specify a particular palette 
                0.f,                             // Alpha threshold
                WICBitmapPaletteTypeCustom       // Palette translation type
                );
        }

        if (SUCCEEDED(hr))
        {
            // Create device render target
            hr = CreateDeviceResources(hWnd);
        }

        // Create D2D bitmap from the converted source bitmap.
        if (SUCCEEDED(hr))
        {
            // Need to release the previous D2DBitmap if there is one.
            // For every new progressive level, we need to create a new D2DBitmap
            SafeRelease(m_pD2DBitmap);
            hr = m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
        }

      

        // Force re-render
        if (SUCCEEDED(hr))
        {
            hr = InvalidateRect(hWnd, NULL, FALSE) ? S_OK : E_FAIL;
        }

        SafeRelease(pProgressive);
    }

    return hr;
}

/******************************************************************
* Creates an open file dialog box and locate the image to decode. *
******************************************************************/

BOOL PicModifier::LocateImageFile(HWND hWnd, LPWSTR pszFileName, DWORD cchFileName)
{
    pszFileName[0] = L'\0';

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = hWnd;
    ofn.lpstrFilter     = L"All Image Files\0"              L"*.gif;*.png;*.jpg;*.jpeg;*.bmp\0"
                          L"Graphics Interchange Format\0"  L"*.gif\0"
                          L"Portable Network Graphics\0"    L"*.png\0"
                          L"JPEG File Interchange Format\0" L"*.jpg;*.jpeg\0"
						  L"BitmapImage\0"					L"*.bmp\0"
                          L"All Files\0"                    L"*.*\0"
                          L"\0";
    ofn.lpstrFile       = pszFileName;
    ofn.nMaxFile        = cchFileName;
    ofn.lpstrTitle      = L"Open Image";
    ofn.Flags           = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    // Display the Open dialog box. 
    return GetOpenFileName(&ofn);
}


/******************************************************************
*  This method creates resources which are bound to a particular  *
*  D2D device. It's all centralized here, in case the resources   *
*  need to be recreated in the event of D2D device loss           *
* (e.g. display change, remoting, removal of video card, etc).    *
******************************************************************/

HRESULT PicModifier::CreateDeviceResources(HWND hWnd)
{
    HRESULT hr = S_OK;

    if (!m_pRT)
    {
        RECT rc;
        hr = GetClientRect(hWnd, &rc) ? S_OK : E_FAIL;

        if (SUCCEEDED(hr))
        {
            // Create a D2D render target properties
            D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();

            // Set the DPI to be the default system DPI to allow direct mapping
            // between image pixels and desktop pixels in different system DPI
            // settings
            renderTargetProperties.dpiX = DEFAULT_DPI;
            renderTargetProperties.dpiY = DEFAULT_DPI;

            // Create a D2D render target
            D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

            hr = m_pD2DFactory->CreateHwndRenderTarget(
                renderTargetProperties,
                D2D1::HwndRenderTargetProperties(hWnd, size),
                &m_pRT
                );
        }
    }

    return hr;
}
/******************************************************************
*  Window message handler                                         *
******************************************************************/

LRESULT CALLBACK PicModifier::s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PicModifier *pThis;
    LRESULT lRet = 0;

    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT pcs = reinterpret_cast<LPCREATESTRUCT> (lParam);
        pThis = reinterpret_cast<PicModifier *> (pcs->lpCreateParams);

        SetWindowLongPtr(hWnd, GWLP_USERDATA, PtrToUlong(pThis));
        lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    else
    {
        pThis = reinterpret_cast<PicModifier *> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (pThis)
        {
            lRet = pThis->WndProc(hWnd, uMsg, wParam, lParam);
        }
        else
        {
            lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    return lRet;
}

LRESULT PicModifier::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HRESULT hr = S_OK;
	::TRACKMOUSEEVENT TrackMouse;
	TrackMouse.cbSize=sizeof(TrackMouse);
	TrackMouse.dwFlags=TME_LEAVE;
	TrackMouse.hwndTrack=hWnd;
    switch (uMsg)
    {
        case WM_COMMAND:
        {
            // Parse the menu selections:
            switch (LOWORD(wParam))
            {
                case IDM_FILE:
                {
                    // Load the new image file
                    hr = CreateProgressiveCtrlFromFile(hWnd);

                    if (SUCCEEDED(hr))
                    {
                        // Create D2DBitmap for rendering at default progessive level
                        hr = CreateD2DBitmapFromProgressiveCtrl(hWnd, 0);
                    }
                    break;
                }
			

                case IDM_EXIT:
                {
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                }
            }
            break;
        }
		case WM_MOUSEWHEEL:
			m_dispf=0;
			if ((short)GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				return magnify(hWnd,LOWORD(lParam),HIWORD(lParam));
			}
			else if ((short)GET_WHEEL_DELTA_WPARAM(wParam) < 0)		
			{
				//if(m_width>m_picW && m_height>m_picH)
				return demagnify(hWnd,LOWORD(lParam),HIWORD(lParam));
			}
			break;
		case WM_LBUTTONDOWN:
			::TrackMouseEvent(&TrackMouse);
			m_dx=LOWORD(lParam);
			m_dy=HIWORD(lParam);
			m_dispf=1;
			break;
		case WM_MOUSEMOVE:
			if(m_dispf==1)
			{
				m_dispx=LOWORD(lParam);
				m_dispy=HIWORD(lParam);
				RECT rect;
				::GetClientRect(hWnd,&rect);
				if(m_dispx == rect.left || m_dispx == rect.right || m_dispy == rect.bottom || m_dispy == rect.top)
				{
					m_dispf=0;
					//RETAILMSG(TRUE,"%d %d\n",m_dispx,m_dispy);
				}
					if(m_width > m_winx || m_height > m_winy)
				{
					displace(hWnd,(int)(m_dispx-m_dx),(int)(m_dispy-m_dy));
				}
				m_dx=m_dispx;
				m_dy=m_dispy;
				
				
			}
			break;
		case WM_MOUSELEAVE:
			
			m_dispf=0;
			break;
		case WM_LBUTTONUP:
			m_dispf=0;
			break;
		case WM_SIZE:
        {
			return OnSize(lParam);
        }
        case WM_PAINT:
        {
            return OnPaint(hWnd);
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return SUCCEEDED(hr)? 0 : 1;
}

/******************************************************************
* Rendering callback using D2D                                    *
******************************************************************/

LRESULT PicModifier::OnPaint(HWND hWnd)
{
    HRESULT hr = S_OK;
    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hWnd, &ps);

    if (hdc)
    {
        // Create render target if not yet created
        hr = CreateDeviceResources(hWnd);

        if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
        {
            m_pRT->BeginDraw();

            m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

            // Clear the background
            m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

            D2D1_SIZE_F rtSize = m_pRT->GetSize();
			if(m_flag==0) // Create a rectangle in size of image
			{		
				m_flag=1;
			}
			else          //draw same image on repaint
			{
				if(m_flag==1)
				{
					if(m_width<m_winx && m_height < m_winy)
					{
						rtSize.width=m_width;
						rtSize.height=m_height;
						m_x=float((m_winx-m_width)/2);
						m_y=float((m_winy-m_height)/2);
					}
					else//to be changed for aspect ratio
					{
						m_picW=rtSize.width=m_width=600*m_aspectratio;
						m_picH=rtSize.height=m_height=600;
						m_x=float((m_winx-m_width)/2);
						m_y=float((m_winy-m_height)/2);
					
					}
					m_flag=2;
				}
				else
				{
					rtSize.width=m_width;
					rtSize.height=m_height;
				}
				
			}
			
			D2D1_RECT_F rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
          // D2DBitmap may have been released due to device loss. 
          // If so, re-create it from the source bitmap
            if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
            {
                m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
            }

            // Draws an image and scales it to the current window size
            if (m_pD2DBitmap)
            {
                m_pRT->DrawBitmap(m_pD2DBitmap, rectangle);
            }

            hr = m_pRT->EndDraw();

            // In case of device loss, discard D2D render target and D2DBitmap.
            // They will be re-create in the next rendering pass
            if (hr == D2DERR_RECREATE_TARGET)
            {
                SafeRelease(m_pD2DBitmap);
                SafeRelease(m_pRT);
                // Force a re-render
                hr = InvalidateRect(hWnd, NULL, TRUE)? S_OK : E_FAIL;
            }
        }

        EndPaint(hWnd, &ps);
    }

    return SUCCEEDED(hr) ? 0 : 1;
}  
LRESULT PicModifier::displace(HWND hWnd,int dispx,int dispy)
{
   HRESULT hr = S_OK;
    

    HDC hdc = GetDC(hWnd);

    if (hdc)
    {
        // Create render target if not yet created
        hr = CreateDeviceResources(hWnd);

        if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
        {
            m_pRT->BeginDraw();

            m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

            // Clear the background
            m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

            D2D1_SIZE_F rtSize ;
			D2D1_RECT_F rectangle;
			rtSize.width=m_width;
			rtSize.height=m_height;
         //flag
			RECT rect;
			::GetClientRect(hWnd,&rect);
			if(m_y <= rect.top && m_x > rect.left)
			{
				m_y+=dispy;
				if(rect.bottom >= (m_y + m_height))
				{
					m_y=m_winy-m_height;
				}
				if(rect.top <=  m_y)
				{
				    m_y=rect.top;
				}
				rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
			}
			else if(m_x <= rect.left && m_y > rect.top) 
			{
				m_x+=dispx;
				if(rect.left <= (m_x))
				{
					m_x=rect.left;
				}
				if(rect.right >= (m_x+m_width))
				{
				    m_x=m_winx-m_width;
				}
				rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);

			}
			else
			{
				m_x+=dispx;
				m_y+=dispy;
				if(rect.bottom >= (m_y + m_height))
				{
					m_y=m_winy-m_height;
				}
				if(rect.top <=  m_y)
				{
				    m_y=rect.top;
				}
				if(rect.left <= (m_x))
				{
					m_x=rect.left;
				}
				if(rect.right >= (m_x+m_width))
				{
				    m_x=m_winx-m_width;
				}
				rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);

			}
				
            // D2DBitmap may have been released due to device loss. 
            // If so, re-create it from the source bitmap
            if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
            {
                m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
			
            }

            // Draws an image and scales it to the current window size
            if (m_pD2DBitmap)
            {
                m_pRT->DrawBitmap(m_pD2DBitmap, rectangle);
            }

            hr = m_pRT->EndDraw();

            // In case of device loss, discard D2D render target and D2DBitmap.
            // They will be re-create in the next rendering pass
            if (hr == D2DERR_RECREATE_TARGET)
            {
                SafeRelease(m_pD2DBitmap);
                SafeRelease(m_pRT);
                // Force a re-render
                hr = InvalidateRect(hWnd, NULL, TRUE)? S_OK : E_FAIL;
            }
        }

		::ReleaseDC(hWnd,hdc);
    }

    return SUCCEEDED(hr) ? 0 : 1;
}  


LRESULT PicModifier::magnify(HWND hWnd,int x,int y)
{
    float r=0,nr=0,l=0,nl=0,offset=0;
	HRESULT hr = S_OK;
	HDC hdc = GetDC(hWnd);
	RECT rect;
	::GetClientRect(hWnd,&rect);
	
	if (hdc)
    {
        // Create render target if not yet created
        hr = CreateDeviceResources(hWnd);
		D2D1_RECT_F rectangle ;
        if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
        {
            m_pRT->BeginDraw();

            m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

            // Clear the background
            m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

            D2D1_SIZE_F rtSize = m_pRT->GetSize();

            // Create a rectangle same size of current window
			if((m_x > rect.left || m_y > rect.top) || ((m_x + m_width) < rect.right  || (m_y+m_height)<rect.bottom) )
			{
				rtSize.width=m_width+=(50*m_aspectratio);
				rtSize.height=m_height+=50;
				m_x=float((m_winx-m_width)/2);
				m_y=float((m_winy-m_height)/2);
				rectangle= D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
			}
			else
			{
				if(x>=m_x && x<=(m_x+m_width) && y>=m_y && y<=(m_y+m_height))
				{
					rtSize.width=m_width+=(50*m_aspectratio);
					rtSize.height=m_height+=50;
					r=(x-m_x);
					nr=r+((50*m_aspectratio)*(r/m_width));
					offset=nr-r;
					m_x=m_x-offset;
					l=(y-m_y);
					nl=l+(50*(l/m_height));
					offset=nl-l;
					m_y=m_y-offset;
					rectangle= D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
				}
				else
				{
					rtSize.width=m_width+=(50*m_aspectratio);
					rtSize.height=m_height+=50;
					m_x=float((m_winx-m_width)/2);
					m_y=float((m_winy-m_height)/2);
					rectangle= D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
				}
			}
			// D2DBitmap may have been released due to device loss. 
            // If so, re-create it from the source bitmap
            if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
            {
                m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
			
            }

            // Draws an image and scales it to the current window size
            if (m_pD2DBitmap)
            {
                m_pRT->DrawBitmap(m_pD2DBitmap, rectangle);
            }

            hr = m_pRT->EndDraw();

            // In case of device loss, discard D2D render target and D2DBitmap.
            // They will be re-create in the next rendering pass
            if (hr == D2DERR_RECREATE_TARGET)
            {
                SafeRelease(m_pD2DBitmap);
                SafeRelease(m_pRT);
                // Force a re-render
                hr = InvalidateRect(hWnd, NULL, TRUE)? S_OK : E_FAIL;
            }
        }

		::ReleaseDC(hWnd,hdc);
    }

    return SUCCEEDED(hr) ? 0 : 1;
} 
LRESULT PicModifier::demagnify(HWND hWnd,int x,int y)
{
   HRESULT hr = S_OK;
   RECT rect;
   ::GetClientRect(hWnd,&rect);
   HDC hdc = GetDC(hWnd);
   if (hdc)
    {
        // Create render target if not yet created
        hr = CreateDeviceResources(hWnd);

        if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
        {
            m_pRT->BeginDraw();

            m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

            // Clear the background
            m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
		    D2D1_SIZE_F rtSize = m_pRT->GetSize();
			D2D1_RECT_F rectangle;
			
			//if(m_winx>=(m_width-50*m_aspectratio) || m_winy >=(m_height-50))
			if(m_winx>=(m_width) || m_winy >=(m_height))
			{
				rtSize.width=m_width-=(50*m_aspectratio);
				rtSize.height=m_height-=50;			
				m_x=float((m_winx-m_width)/2);
				m_y=float((m_winy-m_height)/2);
				rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
			}
			else
			{
				float frac,offset;
				frac=(rect.left-m_x);
				frac=frac/(m_width-m_winx);
				rtSize.width=m_width-=(50*m_aspectratio);
				offset=frac*(m_width-m_winx);
				m_x=rect.left-offset;
				frac=(rect.top-m_y);
				frac=frac/(m_height-m_winy);
				rtSize.height=m_height-=50;	
				offset=frac*(m_height-m_winy);
				m_y=rect.top-offset;
				rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
			}
			//rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
			
            // Create a rectangle same size of current window
			// D2DBitmap may have been released due to device loss. 
            // If so, re-create it from the source bitmap
            if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
            {
                m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
            }

            // Draws an image and scales it to the current window size
            if (m_pD2DBitmap)
            {
                m_pRT->DrawBitmap(m_pD2DBitmap, rectangle);
            }

            hr = m_pRT->EndDraw();

            // In case of device loss, discard D2D render target and D2DBitmap.
            // They will be re-create in the next rendering pass
            if (hr == D2DERR_RECREATE_TARGET)
            {
                SafeRelease(m_pD2DBitmap);
                SafeRelease(m_pRT);
                // Force a re-render
                hr = InvalidateRect(hWnd, NULL, TRUE)? S_OK : E_FAIL;
            }
        }

		::ReleaseDC(hWnd,hdc);
    }

    return SUCCEEDED(hr) ? 0 : 1;

} 
/*LRESULT PicModifier::demagnify(HWND hWnd,int x,int y)
{
   HRESULT hr = S_OK;
   RECT rect;
   ::GetClientRect(hWnd,&rect);
   
   HDC hdc = GetDC(hWnd);
   if (hdc)
    {
        // Create render target if not yet created
        hr = CreateDeviceResources(hWnd);

        if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
        {
            m_pRT->BeginDraw();

            m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

            // Clear the background
            m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

            D2D1_SIZE_F rtSize = m_pRT->GetSize();
			D2D1_RECT_F rectangle;
			rtSize.width=m_width-=(50*m_aspectratio);
			rtSize.height=m_height-=50;
			if((m_x >= rect.left && m_y >= rect.top) && ((m_x + m_width) <= rect.right  && (m_y+m_height)<=rect.bottom) )
			{
				//rtSize.width=m_width-=(25*m_aspectratio);
				//rtSize.height=m_height-=25;
				m_x=float((m_winx-m_width)/2);
				m_y=float((m_winy-m_height)/2);
				rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
			}
			else
			{
				if((rect.left)>=m_x)
					m_x=m_x+(50* m_aspectratio);
			//else	
			//	rtSize.width=m_width-=(25*m_aspectratio);
				if((rect.top)>=m_y)
					m_y=m_y+(50);
			//	else
			//		rtSize.height=m_height-=(25);
			}
			rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
			
            // Create a rectangle same size of current window
			// D2DBitmap may have been released due to device loss. 
            // If so, re-create it from the source bitmap
            if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
            {
                m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
            }

            // Draws an image and scales it to the current window size
            if (m_pD2DBitmap)
            {
                m_pRT->DrawBitmap(m_pD2DBitmap, rectangle);
            }

            hr = m_pRT->EndDraw();

            // In case of device loss, discard D2D render target and D2DBitmap.
            // They will be re-create in the next rendering pass
            if (hr == D2DERR_RECREATE_TARGET)
            {
                SafeRelease(m_pD2DBitmap);
                SafeRelease(m_pRT);
                // Force a re-render
                hr = InvalidateRect(hWnd, NULL, TRUE)? S_OK : E_FAIL;
            }
        }

		::ReleaseDC(hWnd,hdc);
    }

    return SUCCEEDED(hr) ? 0 : 1;

} */
/*LRESULT PicModifier::demagnify(HWND hWnd,int x,int y)
{
   float r=0,nr=0,l=0,nl=0,offset=0;
   RECT rect;
	::GetClientRect(hWnd,&rect);
   HRESULT hr = S_OK;
   HDC hdc = GetDC(hWnd);
   if (hdc)
    {
        // Create render target if not yet created
        hr = CreateDeviceResources(hWnd);

        if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
        {
            m_pRT->BeginDraw();

            m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

            // Clear the background
            m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

            D2D1_SIZE_F rtSize = m_pRT->GetSize();
			D2D1_RECT_F rectangle;
			

            // Create a rectangle same size of current window
			if((m_x >= rect.left && m_y >= rect.top) && ((m_x + m_width) <= rect.right  && (m_y+m_height)<=rect.bottom) )
			{
				rtSize.width=m_width-=(100*m_aspectratio);
				rtSize.height=m_height-=100;
				m_x=float((m_winx-m_width)/2);
				m_y=float((m_winy-m_height)/2);
				rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
			}
			else
			{	
				if(x>=m_x && x<=(m_x+m_width) && y>=m_y && y<=(m_y+m_height))
				{
					rtSize.width=m_width-=(100*m_aspectratio);
					rtSize.height=m_height-=100;
					r=(x-m_x);
					nr=r-((100*m_aspectratio)*(r/m_width));
					offset=r-nr;
					m_x=m_x+offset;
					l=(y-m_y);
					nl=l-(100*(l/m_height));
					offset=l-nl;
					m_y=m_y+offset;
					rectangle= D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
				}
				else
				{
					rtSize.width=m_width-=(100*m_aspectratio);
					rtSize.height=m_height-=100;
					m_x=float((m_winx-m_width)/2);
					m_y=float((m_winy-m_height)/2);
					rectangle = D2D1::RectF(m_x, m_y, rtSize.width+m_x, rtSize.height+m_y);
				}
			}
			// D2DBitmap may have been released due to device loss. 
            // If so, re-create it from the source bitmap
            if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
            {
                m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
            }

            // Draws an image and scales it to the current window size
            if (m_pD2DBitmap)
            {
                m_pRT->DrawBitmap(m_pD2DBitmap, rectangle);
            }

            hr = m_pRT->EndDraw();

            // In case of device loss, discard D2D render target and D2DBitmap.
            // They will be re-create in the next rendering pass
            if (hr == D2DERR_RECREATE_TARGET)
            {
                SafeRelease(m_pD2DBitmap);
                SafeRelease(m_pRT);
                // Force a re-render
                hr = InvalidateRect(hWnd, NULL, TRUE)? S_OK : E_FAIL;
            }
        }

		::ReleaseDC(hWnd,hdc);
    }

    return SUCCEEDED(hr) ? 0 : 1;

} */ 

/******************************************************************
* Resizing callback                                               *
******************************************************************/

LRESULT PicModifier::OnSize(LPARAM lParam)
{
    LRESULT lRet = 1;

   
    // Resize rendering target
    D2D1_SIZE_U size = D2D1::SizeU(LOWORD(lParam), HIWORD(lParam));

    if (m_pRT)
    {
        // If we couldn't resize, release the device and we'll recreate it
        // during the next render pass.
        if (FAILED(m_pRT->Resize(size)))
        {
            SafeRelease(m_pRT);
            SafeRelease(m_pD2DBitmap);
        }
        else
        {
            lRet = 0;
        }
    }

    return lRet;
}

