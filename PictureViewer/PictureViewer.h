

#pragma once

#include "Resource.h"

const FLOAT DEFAULT_DPI = 96.f;   // Default DPI that maps image resolution directly to screen resoltuion

/******************************************************************
*                                                                 *
*  PicModifier                                                        *
*                                                                 *
******************************************************************/

class PicModifier
{
public:
    PicModifier();
    ~PicModifier();

    HWND Initialize(HINSTANCE hInstance);

private:
    BOOL    LocateImageFile(HWND hWnd, LPWSTR pszFileName, DWORD cbFileName);
    HRESULT CreateProgressiveCtrlFromFile(HWND hWnd);
    HRESULT CreateD2DBitmapFromProgressiveCtrl(HWND hWnd, UINT uLevel);
    HRESULT CreateDeviceResources(HWND hWnd);
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnPaint(HWND hWnd);
    LRESULT OnSize(LPARAM lParam);
	LRESULT magnify(HWND hWnd,int x,int y);
	LRESULT demagnify(HWND hWnd,int x,int y);
	LRESULT displace(HWND hWnd,int m_dispx,int m_dispy);
    static LRESULT CALLBACK s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    int m_flag,m_dispf;
	float m_x,m_y,m_width,m_height,m_aspectratio,m_winx,m_winy,m_dispx,m_dispy,m_picW,m_picH,m_dx,m_dy;
	
    HINSTANCE                    m_hInst;
    IWICImagingFactory          *m_pIWICFactory;
    UINT                         m_uCurrentLevel;
    UINT                         m_uLevelCount;
    ID2D1Factory                *m_pD2DFactory;
    ID2D1HwndRenderTarget       *m_pRT;
    IWICBitmapFrameDecode       *m_pSourceFrame;
    ID2D1Bitmap                 *m_pD2DBitmap;
    IWICFormatConverter         *m_pConvertedSourceBitmap;
};