// ImageViewer.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "ImageViewer.h"
#include "ImageView.h"
#include <imaging.h>
#include <initguid.h>
#include <imgguids.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define REDCOLOR		(RGB(255, 0, 0))
#define GREENCOLOR		(RGB(0, 255, 0))
#define BLACKCOLOR		(RGB(0, 0, 0))
CImageView::CImageView()
{
	m_hDC = NULL;
	m_hWND = NULL;
	m_pImgFactory = NULL;
	m_pImage = NULL;
	m_pBitmapImage = NULL;
	m_pBitMapInfo = NULL;
	//CoInitializeEx(NULL, COINIT_MULTITHREADED);
}
CImageView::~CImageView(){	CleanUp(); }

BOOL CImageView::Init(LPCTSTR pszFileName, HWND hWnd)
{  
	BOOL rc = TRUE;
	
	m_hWND = hWnd;

	if(m_hWND)
	{
		m_hDC = GetDC(m_hWND);
		if(m_hDC == NULL)
		{
			rc = FALSE;
			RETAILMSG(1, (TEXT(" ERROR:Fails to get DC!!! \r\n")));
		}
	}
	else
	{
		rc = FALSE;
		RETAILMSG(1, (TEXT("ERROR: NULL window handle!!! \r\n")));
	}

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (SUCCEEDED(CoCreateInstance (CLSID_ImagingFactory,
									NULL,
									CLSCTX_INPROC_SERVER,
									IID_IImagingFactory,
									(void **)&m_pImgFactory)))
	{
		// Load the image from the JPG file.
		if (FAILED(m_pImgFactory->CreateImageFromFile(pszFileName, &m_pImage)))
		{
			rc = FALSE;
			RETAILMSG(1, (TEXT(" ERROR: Unable to open the File \r\n")));
		}


	}
	else
	{
	   rc = FALSE;
	   RETAILMSG(1, (TEXT(" ERROR: CoCreateInstance Failed for CLSID_ImagingFactory \r\n")));
	}

	return rc;
}

BOOL CImageView::CleanUp()
{
	BOOL rc = TRUE;

	m_hDC = NULL;

	if(m_pZoomLevels)
	{
		delete(m_pZoomLevels);
		m_pZoomLevels = NULL;
	}

	if(m_pThumbImage)
	{
		m_pThumbImage->Release();
		m_pThumbImage = NULL;
	}

	if(m_pBitmapImage)
		m_pBitmapImage->Release();

	if(m_pImage)
		m_pImage->Release();

	if(m_pImgFactory)
		m_pImgFactory->Release();

	m_pImgFactory = NULL;
	m_pBitMapInfo = NULL;
	m_pBitmapImage = NULL;
	m_pImage = NULL;

	CoUninitialize();
	
	return rc;
}

BOOL CImageView::CalculateDestImageSize()
{
	double ImageRatio;

	if(m_BitmapImageWidth <= m_ScreenWidth && m_BitmapImageHeight <= m_ScreenHeight)
	{
		m_RequiredWidth = m_BitmapImageWidth;
		m_RequiredHeight = m_BitmapImageHeight;
	}
	else
	{
		if(m_BitmapImageWidth > m_ScreenWidth)
		{
			m_RequiredWidth = m_ScreenWidth;
			ImageRatio = (double)m_BitmapImageWidth/(double)m_BitmapImageHeight;	
			m_RequiredHeight =(int) ((double)m_RequiredWidth/ImageRatio);
			
			while(m_RequiredHeight > m_ScreenHeight)
			{
				m_RequiredWidth--;
				ImageRatio = (double)m_BitmapImageWidth/(double)m_BitmapImageHeight;	
				m_RequiredHeight =(int) ((double)m_RequiredWidth/ImageRatio);
			}

			m_RequiredWidth = (m_RequiredHeight * m_BitmapImageWidth) / m_BitmapImageHeight;
		}
		else
		{
			m_RequiredHeight = m_ScreenHeight;
			ImageRatio = (double)m_BitmapImageHeight/(double)m_BitmapImageWidth;
			m_RequiredWidth = int ((double)m_RequiredHeight/ImageRatio);

			while(m_RequiredWidth > m_ScreenWidth)
			{
				m_RequiredHeight--;
				ImageRatio = (double)m_BitmapImageHeight/(double)m_BitmapImageWidth;
				m_RequiredWidth = int ((double)m_RequiredHeight/ImageRatio);
			}

			m_RequiredHeight = (m_RequiredWidth * m_BitmapImageHeight) / m_BitmapImageWidth;
		}
	}

	m_DestImageWidth = m_RequiredWidth;
	m_DestImageHeight = m_RequiredHeight;
	
	m_DestXStart = ((m_ScreenWidth - m_RequiredWidth)/2);
	m_DestYStart = ((m_ScreenHeight - m_RequiredHeight)/2);

	m_DestImageCentreX = (m_DestXStart + m_DestImageWidth)/2;
	m_DestImageCentreY = (m_DestYStart + m_DestImageHeight)/2;

	RETAILMSG(1,(_T("GetInitialImageSize: Req. Width = %d Req. Height = %d\r\n"), m_RequiredWidth, m_RequiredHeight));

	return TRUE;
}

#define		IMAGE_DEC_STEP		300

BOOL CImageView::ShowImage()
{
	BOOL rc = TRUE;
	double imageRatio;

	if(!m_hDC)
	{
		RETAILMSG(1,(_T("ShowImage: DC Invalid\r\n")));
		return FALSE;
	}

	if(!GetScreenSize())
	{
		rc = FALSE;
		RETAILMSG(1,(_T("ShowImage: GetScreenSize Failed\r\n")));
		return rc;
	}

	if(FAILED(m_pImage->GetImageInfo(&m_ImageInfo)))
	{
		rc = FALSE;
		RETAILMSG(1,(_T("ShowImage: GetImageInfo Failed\r\n")));
		return rc;
	}

	m_BitmapImageWidth = m_ImageInfo.Width;
	m_BitmapImageHeight = m_ImageInfo.Height;
	
	imageRatio = (double)m_BitmapImageWidth/(double)m_BitmapImageHeight;

	while(1)
	{
		RETAILMSG(1,(_T("ShowImage: Trying to load - %d x %d\r\n"), m_BitmapImageWidth, m_BitmapImageHeight));

		if(SUCCEEDED(m_pImgFactory->CreateBitmapFromImage(m_pImage, m_BitmapImageWidth, m_BitmapImageHeight, PixelFormat16bppRGB565, InterpolationHintDefault, &m_pBitmapImage)))
		{
			RETAILMSG(1,(_T("ShowImage: Got Image Successfully\r\n")));
			break;
		}
		else
		{
			m_BitmapImageHeight -= IMAGE_DEC_STEP;
			m_BitmapImageWidth = (UINT)(((double)m_BitmapImageHeight) * imageRatio);

			if(m_BitmapImageWidth < 100 || m_BitmapImageHeight < 100)
			{
				RETAILMSG(1,(_T("ShowImage: Failed to create bitmap from image\r\n")));
				return FALSE;
			}

			RETAILMSG(1,(_T("ShowImage: Failed Retrying with reduced size\r\n")));
			Sleep(100);
		}
	}
	
	RETAILMSG(1,(_T("ShowImage: Width = %d Height = %d\r\n"), m_BitmapImageWidth, m_BitmapImageHeight));

	m_ImageRatio = (double)m_BitmapImageWidth/(double)m_BitmapImageHeight;

	m_SrcXStart = 0;
	m_SrcYStart = 0;
	m_SrcImageWidth = m_BitmapImageWidth;
	m_SrcImageHeight = m_BitmapImageHeight;
	
	if(!CalculateDestImageSize())
	{
		rc = FALSE;
		return rc;
	}

	RECT rect;
	SetRect(&rect, 0, 0, m_BitmapImageWidth, m_BitmapImageHeight);

	m_pBitmapImage->LockBits(&rect, ImageLockModeRead, PixelFormat16bppRGB565, &m_Bitmapdata);
	CreateBitMapInfo();
	m_pBitmapImage->UnlockBits(&m_Bitmapdata);

	rc = DrawImage();			
	
	return rc;
}
BOOL CImageView::DrawImage()
{
	BOOL rc = TRUE;

	if(m_Bitmapdata.Scan0)
	{
		//RETAILMSG(1, (TEXT("StretchDIBits: DWidth = %d DHeight = %d DXStart = %d DYStart = %d!!! \r\n"), m_DestImageWidth, m_DestImageHeight, m_DestXStart, m_DestYStart));
		RETAILMSG(1, (TEXT("StretchDIBits: SW = %d SH = %d SX = %d SY = %d\r\n"), m_SrcImageWidth, m_SrcImageHeight, m_SrcXStart, m_SrcYStart));
		if(!StretchDIBits(	m_hDC,
							m_DestXStart, m_DestYStart,
							m_DestImageWidth, m_DestImageHeight,
							m_SrcXStart, m_SrcYStart,
							m_SrcImageWidth, m_SrcImageHeight,
							(UINT8*)m_Bitmapdata.Scan0 ,
							m_pBitMapInfo,
							DIB_RGB_COLORS,
							SRCCOPY))
		{
			RETAILMSG(1, (TEXT("DrawImage: Fail to show the image !!! \r\n")));
			rc = FALSE;
		}
	}
	else
	{
		RETAILMSG(1, (TEXT("DrawImage: Bitmap Data is NULL !!! \r\n")));
		rc = FALSE;
	}

	return rc;
}

BOOL CImageView::ReDrawImage()
{
	return DrawImage();
}

BOOL CImageView::SetZoomCentre(int cx, int cy)
{
	if(cx > m_DestImageWidth)
		cx = m_DestImageWidth;
	else if(cx < m_DestXStart)
		cx = m_DestXStart;

	if(cy > m_DestImageHeight)
		cy = m_DestImageHeight;
	else if(cy < m_DestYStart)
		cy = m_DestYStart;

	cx -= m_DestXStart;
	cy -= m_DestYStart;

	m_ZoomCentreX = m_SrcXStart + (UINT)((double)cx * (double)m_SrcImageWidth/(double)m_DestImageWidth);
	m_ZoomCentreY = m_SrcYStart + (UINT)((double)cy * (double)m_SrcImageHeight/(double)m_DestImageHeight);

	//m_ZoomCentreX = m_SrcXStart + (UINT)((double)cx * (double)m_BitmapImageWidth/(double)m_DestImageWidth);
	//m_ZoomCentreY = m_SrcYStart + (UINT)((double)cy * (double)m_BitmapImageHeight/(double)m_DestImageHeight);

	return TRUE;
}

#define ZOOM_Y_STEP_PIXEL			6

#define STEP_X						100
#define STEP_Y						75

BOOL CImageView::ZoomInImage(int cx, int cy)
{
	BOOL rc = TRUE;
	HRESULT hr = S_OK;
	BOOL bFullSize = FALSE;
	UINT CurrCentreX, CurrCentreY;
	UINT NextCentreX, NextCentreY;

	if(m_SrcImageWidth == m_DestImageWidth || m_SrcImageHeight == m_DestImageHeight)
	{
		RETAILMSG(1, (TEXT("ZoomInImage: MAX ZOOM!!! \r\n")));
		return FALSE;
	}

	if(cx == -1 || cy == -1)
	{
		m_SrcImageHeight -= (ZOOM_Y_STEP_PIXEL * 2);
		m_SrcImageWidth -= (UINT)((ZOOM_Y_STEP_PIXEL * m_ImageRatio) * 2);
		
		m_SrcXStart += (UINT)(ZOOM_Y_STEP_PIXEL * m_ImageRatio);
		m_SrcYStart += ZOOM_Y_STEP_PIXEL;
		
		if(m_SrcImageHeight < m_DestImageHeight || m_SrcImageWidth < m_DestImageWidth)
		{
			RETAILMSG(1, (TEXT("TEST\r\n")));
			m_SrcImageWidth = m_DestImageWidth;
			m_SrcImageHeight = m_DestImageHeight;
		}
	}
	else
	{
		CurrCentreX = m_SrcXStart + (m_SrcImageWidth / 2);
		CurrCentreY = m_SrcYStart + (m_SrcImageHeight / 2);

		m_SrcImageHeight -= (ZOOM_Y_STEP_PIXEL * 2);
		m_SrcImageWidth -= (UINT)((ZOOM_Y_STEP_PIXEL * m_ImageRatio) * 2);

		if(m_SrcImageHeight < m_DestImageHeight || m_SrcImageWidth < m_DestImageWidth)
		{
			RETAILMSG(1, (TEXT("TEST\r\n")));
			m_SrcImageWidth = m_DestImageWidth;
			m_SrcImageHeight = m_DestImageHeight;
		}

		NextCentreX = (CurrCentreX + m_ZoomCentreX)/2;
		NextCentreY = (CurrCentreY + m_ZoomCentreY)/2;

		m_SrcXStart = NextCentreX - (m_SrcImageWidth / 2);
		m_SrcYStart = NextCentreY - (m_SrcImageHeight / 2);

		if(m_SrcXStart < 0)
		{
			m_SrcXStart = 0;
		}

		if((m_SrcXStart + m_SrcImageWidth) > m_BitmapImageWidth)
		{
			m_SrcXStart = m_BitmapImageWidth - m_SrcImageWidth;
		}

		if(m_SrcYStart < 0)
		{
			m_SrcYStart = 0;
		}

		if((m_SrcYStart + m_SrcImageHeight) > m_BitmapImageHeight)
		{
			m_SrcYStart = m_BitmapImageHeight - m_SrcImageHeight;
		}
	}
	
	
	//RETAILMSG(1, (TEXT("Z+\r\n")));

	rc = DrawImage();

	return rc;
}

BOOL CImageView::ZoomOutImage(int cx, int cy)
{
	BOOL rc = TRUE;

	if(m_SrcImageWidth == m_BitmapImageWidth || m_SrcImageHeight == m_BitmapImageHeight)
	{
		RETAILMSG(1,(_T("ZoomOutImage: MIN Zoom\r\n")));
		return FALSE;
	}

	if(1)//cx == -1 || cy == -1)
	{
		m_SrcYStart -= ZOOM_Y_STEP_PIXEL;
		m_SrcImageHeight += (ZOOM_Y_STEP_PIXEL * 2);

		if(m_SrcYStart < 0)
		{
			m_SrcYStart = 0;
		}

		if(m_SrcImageHeight > m_BitmapImageHeight)
		{
			m_SrcImageHeight = m_BitmapImageHeight;
		}

		if((m_SrcYStart + m_SrcImageHeight) > m_BitmapImageHeight)
		{
			m_SrcYStart = m_SrcYStart - ((m_SrcYStart + m_SrcImageHeight) - m_BitmapImageHeight);
		}

		m_SrcXStart -= (UINT)(ZOOM_Y_STEP_PIXEL * m_ImageRatio);
		m_SrcImageWidth += (UINT)((ZOOM_Y_STEP_PIXEL * m_ImageRatio) * 2);

		if(m_SrcXStart < 0)
		{
			m_SrcXStart = 0;
		}

		if(m_SrcImageWidth > m_BitmapImageWidth)
		{
			m_SrcXStart = 0;
			m_SrcImageWidth = m_BitmapImageWidth;
		}

		if((m_SrcXStart + m_SrcImageWidth) > m_BitmapImageWidth)
		{
			m_SrcXStart = m_SrcXStart - ((m_SrcXStart + m_SrcImageWidth) - m_BitmapImageWidth);
		}
	}
	else
	{
		if(cx > m_DestImageWidth)
			cx = m_DestImageWidth;
		else if(cx < m_DestXStart)
			cx = m_DestXStart;

		if(cy > m_DestImageHeight)
			cy = m_DestImageHeight;
		else if(cy < m_DestYStart)
			cy = m_DestYStart;
	}
	RETAILMSG(1, (TEXT("Z-\r\n")));
	rc = DrawImage();

	return rc;
}

BOOL CImageView::MoveImage(INT XPos, INT YPos)
{
	BOOL rc = TRUE;

	if(!m_pBitmapImage)
	{
		RETAILMSG(1,(_T("MoveImage: Bitmap Invalid\r\n")));
		return FALSE;
	}

	if((m_SrcImageWidth == m_BitmapImageWidth) || (m_SrcImageHeight == m_BitmapImageHeight))
	{
		RETAILMSG(1,(_T("MoveImage: Image is Fit\r\n")));
		return FALSE;
	}

	if(m_SrcImageWidth < m_BitmapImageWidth)
	{			
		m_SrcXStart = m_SrcXStart - XPos;

		if((INT)m_SrcXStart < 0)
		{
			m_SrcXStart = 0;
		}
		else if(m_SrcXStart > (m_BitmapImageWidth - m_SrcImageWidth))
		{
			m_SrcXStart = (m_BitmapImageWidth - m_SrcImageWidth);
		}
	}

	if(m_SrcImageHeight < m_BitmapImageHeight)
	{
		m_SrcYStart = m_SrcYStart - YPos;

		if((INT)m_SrcYStart < 0)
		{
			m_SrcYStart = 0;
		}
		else if(m_SrcYStart > (m_BitmapImageHeight - m_SrcImageHeight))
		{
			m_SrcYStart = (m_BitmapImageHeight - m_SrcImageHeight);
		}
	}

	RETAILMSG(1, (TEXT("Move\r\n")));
	rc = DrawImage();

	return rc;
}

BOOL CImageView::CreateBitMapInfo()
{
	BOOL rc = TRUE;
	
    m_pBitMapInfo =(BITMAPINFO*) new char[sizeof(BITMAPINFO) + 4];

	if(m_pBitMapInfo)
	{
		//Now write the BITMAPINFOHEADER
		m_pBitMapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pBitMapInfo->bmiHeader.biWidth = m_Bitmapdata.Width;
		m_pBitMapInfo->bmiHeader.biHeight = -(LONG)m_Bitmapdata.Height;
		m_pBitMapInfo->bmiHeader.biPlanes = 1;
		m_pBitMapInfo->bmiHeader.biBitCount = 16;
		m_pBitMapInfo->bmiHeader.biCompression = BI_BITFIELDS;
		m_pBitMapInfo->bmiHeader.biSizeImage = m_Bitmapdata.Width*m_Bitmapdata.Height;
		m_pBitMapInfo->bmiHeader.biClrImportant = 0;
		m_pBitMapInfo->bmiHeader.biClrUsed = 1;
		
		unsigned long *ptr = (unsigned long *)(m_pBitMapInfo->bmiColors);
		 ptr[0] = 0xf800;
		 ptr[1] = 0x07e0;
		 ptr[2] = 0x001f;
			
	}
	else
		rc = FALSE;
		
	return rc;
}

BOOL CImageView::CalculateInitialParameters()
{
	BOOL rc = TRUE;

	if(!GetScreenSize())
	{
		rc = FALSE;
		return rc;
	}

	if(FAILED(m_pImage->GetImageInfo(&m_ImageInfo)))
	{
		rc = FALSE;
		return rc;
	}

	if(!GetInitialImageSize())
	{
		rc = FALSE;
		return rc;
	}

	if(!CalculateZoomLevels())
	{
		rc = FALSE;
		return rc;
	}
	
	return rc;
}

BOOL CImageView::GetScreenSize()
{
	BOOL rc = TRUE;
	
	if(GetWindowRect(m_hWND, &m_ScreenRect))
	{
		m_ScreenWidth = m_ScreenRect.right - m_ScreenRect.left;
		m_ScreenHeight = m_ScreenRect.bottom - m_ScreenRect.top;
	}
	else
		rc = FALSE;

	RETAILMSG(1,(_T("GetScreenSize: Width = %d Height = %d\r\n"), m_ScreenWidth, m_ScreenHeight));

	return rc;
}

BOOL CImageView::GetInitialImageSize()
{
	double ImageRatio;

	if(m_ImageInfo.Width <= m_ScreenWidth && m_ImageInfo.Height <= m_ScreenHeight)
	{
		m_RequiredWidth = m_ImageInfo.Width;
		m_RequiredHeight = m_ImageInfo.Height;
	}
	else
	{
		if(m_ImageInfo.Width > m_ScreenWidth)
		{
			m_RequiredWidth = m_ScreenWidth;
			ImageRatio = (double)m_ImageInfo.Width/(double)m_ImageInfo.Height;	
			m_RequiredHeight =(int) ((double)m_RequiredWidth/ImageRatio);
			
			while(m_RequiredHeight > m_ScreenHeight)
			{
				m_RequiredWidth--;
				ImageRatio = (double)m_ImageInfo.Width/(double)m_ImageInfo.Height;	
				m_RequiredHeight =(int) ((double)m_RequiredWidth/ImageRatio);
			}

			m_RequiredWidth = (m_RequiredHeight * m_ImageInfo.Width) / m_ImageInfo.Height;
		}
		else
		{
			m_RequiredHeight = m_ScreenHeight;
			ImageRatio = (double)m_ImageInfo.Height/(double)m_ImageInfo.Width;
			m_RequiredWidth = int ((double)m_RequiredHeight/ImageRatio);

			while(m_RequiredWidth > m_ScreenWidth)
			{
				m_RequiredHeight--;
				ImageRatio = (double)m_ImageInfo.Height/(double)m_ImageInfo.Width;
				m_RequiredWidth = int ((double)m_RequiredHeight/ImageRatio);
			}

			m_RequiredHeight = (m_RequiredWidth * m_ImageInfo.Height) / m_ImageInfo.Width;
		}
	}

	m_RequiredXstart = ((m_ScreenWidth - m_RequiredWidth)/2);
	m_RequiredYstart = ((m_ScreenHeight - m_RequiredHeight)/2);

	RETAILMSG(1,(_T("GetInitialImageSize: Req. Width = %d Req. Height = %d\r\n"), m_RequiredWidth, m_RequiredHeight));

	return TRUE;
}

BOOL CImageView::CalculateZoomLevels()
{
	m_NoofZoomLevels = m_ImageInfo.Width / m_RequiredWidth;
	if(m_ImageInfo.Width % m_RequiredWidth)
	{
		m_NoofZoomLevels++;
	}

	RETAILMSG(1,(_T("CalculateZoomLevels: Zoom Levels = %d\r\n"), m_NoofZoomLevels));

	m_pZoomLevels = new ZOOMLEVELS[m_NoofZoomLevels];

	m_pZoomLevels[0].Width = m_RequiredWidth;
	m_pZoomLevels[0].Height = m_RequiredHeight;

	for(UINT Cnt = 1; Cnt < m_NoofZoomLevels; Cnt++)
	{
		m_pZoomLevels[Cnt].Height = (m_RequiredHeight * (Cnt + 1));
		m_pZoomLevels[Cnt].Width = (m_RequiredWidth * (Cnt + 1)) ;
		if(m_pZoomLevels[Cnt].Width > m_ImageInfo.Width || m_pZoomLevels[Cnt].Height > m_ImageInfo.Height)
		{
			m_pZoomLevels[Cnt].Height = m_ImageInfo.Height;
			m_pZoomLevels[Cnt].Width = m_ImageInfo.Width;
		}
	}

	m_CurrentZoomLevel = 0;

	return TRUE;
}


BOOL CImageView::CreateZoomImage(BOOL bFullSize)
{
	BOOL rc = TRUE;
	HRESULT hr = S_OK;

	RETAILMSG(1,(_T("CreateZoomImage: Create Bitmap Width = %d Height = %d\r\n"), m_ReviewImageWidth, m_ReviewImageHeight));

	if(!bFullSize)
	{
		if(m_pThumbImage)
		{
			m_pThumbImage->Release();
			m_pThumbImage = NULL;
		}

		if(FAILED(m_pImage->GetThumbnail(m_ReviewImageWidth, m_ReviewImageHeight, &m_pThumbImage)))
		{
			RETAILMSG(1, (TEXT(" CreateZoomImage: Failed to create thumb image\r\n")));
			rc = FALSE;
			return rc;
		}

		if(m_pBitmapImage)
		{
			m_pBitmapImage->Release();
			m_pBitmapImage = NULL;
		}

		if(FAILED(m_pImgFactory->CreateBitmapFromImage(m_pThumbImage, m_ReviewImageWidth, m_ReviewImageHeight, PixelFormat16bppRGB565, InterpolationHintBicubic, &m_pBitmapImage)))
		{
			RETAILMSG(1, (TEXT(" CreateZoomImage: Failed to create Bitmap \r\n")));
			rc = FALSE;
			return rc;
		}
	}
	else
	{
		if(m_pBitmapImage)
		{
			m_pBitmapImage->Release();
			m_pBitmapImage = NULL;
		}

		if(FAILED(m_pImgFactory->CreateBitmapFromImage(m_pImage, m_ReviewImageWidth, m_ReviewImageHeight, PixelFormat16bppRGB565, InterpolationHintBicubic, &m_pBitmapImage)))
		{
			RETAILMSG(1, (TEXT(" CreateZoomImage: Failed to create Bitmap \r\n")));
			rc = FALSE;
			return rc;
		}
	}

	RECT rect;
	SetRect(&rect, 0, 0, m_ReviewImageWidth, m_ReviewImageHeight);

	hr = m_pBitmapImage->LockBits(&rect, ImageLockModeRead, PixelFormat16bppRGB565, &m_Bitmapdata);
	if(FAILED(hr))
	{
		rc = FALSE;
		goto cleanup;
		RETAILMSG(1, (TEXT("CreateZoomImage: Failed to Lock Bits hr = 0x%x Error = %d\r\n"), hr, GetLastError()));
	}

	CreateBitMapInfo();

	hr = m_pBitmapImage->UnlockBits(&m_Bitmapdata);
	if(FAILED(hr))
	{
		rc = FALSE;
		goto cleanup;
		RETAILMSG(1, (TEXT("CreateZoomImage: Failed to Unlock Bits hr = 0x%x error = %d\r\n"), hr, GetLastError()));
	}
	
	if(m_ReviewImageWidth < m_ScreenWidth)
	{
		m_DestXStart = (m_ScreenWidth - m_ReviewImageWidth)/2;
		m_DestImageWidth = m_ReviewImageWidth;
		m_SrcXStart = 0;
		m_SrcImageWidth = m_Bitmapdata.Width;
	}
	else
	{
		m_DestXStart = 0;
		m_DestImageWidth = m_ScreenWidth;
		m_SrcXStart = (m_ReviewImageWidth - m_ScreenWidth)/2;
		m_SrcImageWidth = m_ScreenWidth;
	}

	if(m_ReviewImageHeight < m_ScreenHeight)
	{
		m_DestYStart = (m_ScreenHeight - m_ReviewImageHeight)/2;
		m_DestImageHeight = m_ReviewImageHeight;
		m_SrcYStart = 0;
		m_SrcImageHeight = m_Bitmapdata.Height;
	}
	else
	{
		m_DestYStart = 0;
		m_DestImageHeight = m_ScreenHeight;
		m_SrcYStart = (m_ReviewImageHeight - m_ScreenHeight)/2;
		m_SrcImageHeight = m_ScreenHeight;
	}

cleanup:

	return rc;
}
