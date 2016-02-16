// yuytobmpconversion.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "conio.h"
#include "stdlib.h"
#define UINT16 short
#define UINT32 long
#define MINIMUMOFFSET				10
#define BITMAPSIGNATURE				0x4d42
#define BMPHEADERSIZE				54
#define CLIP(X) (X>255)?255:(X<0)?0:X
typedef struct BMPHEADERINFO
{
	UINT16 Signature;
	UINT32 Size;
	UINT16 Reserved1;
	UINT16 Reserved2;
	UINT32 OffsetToStartOfImage;
	UINT32 BitmapInfoHeaderSize;
	UINT32 ImageWidth;
	UINT32 ImageHeight;
	UINT16 NumberOfPlanes;
	UINT16 NumberOfBitsPerPixel;
	UINT32 CompressionType;
	UINT32 SizeOfImageDataIncludingPadding;
	UINT32 HorizontalResolution;
	UINT32 VerticalResolution;
	UINT32 NumberOfColorsInImage;
	UINT32 NumberOfImportantColors;

}BMPHEADER;
BMPHEADER m_BmpHeader;
void InitBmpHeader(int Width,int Height)
{
	m_BmpHeader.Signature						= BITMAPSIGNATURE;
	m_BmpHeader.Size							= (Width*3 + (Width % 4))*Height +BMPHEADERSIZE	;
	m_BmpHeader.Reserved1						= 0;
	m_BmpHeader.Reserved2						= 0;
	m_BmpHeader.OffsetToStartOfImage			= BMPHEADERSIZE	;
	m_BmpHeader.BitmapInfoHeaderSize			= 40;
	m_BmpHeader.ImageWidth						= Width;
	m_BmpHeader.ImageHeight						= Height;
	m_BmpHeader.NumberOfPlanes					= 1;
	m_BmpHeader.NumberOfBitsPerPixel			= 24;
	m_BmpHeader.CompressionType					= 0;
	m_BmpHeader.SizeOfImageDataIncludingPadding = m_BmpHeader.Size-BMPHEADERSIZE;
	m_BmpHeader.HorizontalResolution			= 0;
	m_BmpHeader.VerticalResolution				= 0;
	m_BmpHeader.NumberOfColorsInImage			= 0;
	m_BmpHeader.NumberOfImportantColors			= 0;
}
void WriteBmpHeader(FILE * DestinationImage)
{
	fwrite(&m_BmpHeader.Signature,2,1,DestinationImage);						
	fwrite(&m_BmpHeader.Size,4,1,DestinationImage);		 				
	fwrite(&m_BmpHeader.Reserved1,2,1,DestinationImage);
	fwrite(&m_BmpHeader.Reserved2,2,1,DestinationImage);						
	fwrite(&m_BmpHeader.OffsetToStartOfImage,4,1,DestinationImage);			
	fwrite(&m_BmpHeader.BitmapInfoHeaderSize,4,1,DestinationImage);			
	fwrite(&m_BmpHeader.ImageWidth,4,1,DestinationImage);						
	fwrite(&m_BmpHeader.ImageHeight,4,1,DestinationImage);						
	fwrite(&m_BmpHeader.NumberOfPlanes,2,1,DestinationImage);					
	fwrite(&m_BmpHeader.NumberOfBitsPerPixel,2,1,DestinationImage);			
	fwrite(&m_BmpHeader.CompressionType,4,1,DestinationImage);					
	fwrite(&m_BmpHeader.SizeOfImageDataIncludingPadding,4,1,DestinationImage); 
	fwrite(&m_BmpHeader.HorizontalResolution,4,1,DestinationImage);			
	fwrite(&m_BmpHeader.VerticalResolution,4,1,DestinationImage);			
	fwrite(&m_BmpHeader.NumberOfColorsInImage,4,1,DestinationImage);	
	fwrite(&m_BmpHeader.NumberOfImportantColors,4,1,DestinationImage);	
}

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *Source = NULL,*Destination = NULL;
	char Sourcefile[255], Destinationfile[255], width = 0 , height = 0;
	printf("Enter the input file with full path:");
	scanf("%s",Sourcefile);
	printf("Enter Source width and Height:");
	scanf("%d %d", &width,&height);
	printf("Enter the output file with full path:");
	scanf("%s",Destinationfile);
	Source = fopen(Sourcefile,"rb");
	Destination = fopen(Destinationfile,"wb");
	InitBmpHeader(width,height);
	WriteBmpHeader(Destination);
	int dwWidthIn = width ;
	int dwHeightIn = height;
	int lStrideOut = dwWidthIn * 3;
	int lStrideIn = dwWidthIn * 2;
	int dwWidthOut = dwWidthIn;
	int   Y1, U, V, Y2;
	char Blue,Green,Red ;
	for(int Col = 0; Col<dwHeightIn; Col++)
	{
	
		for(int Row = 0, DestRow =0; DestRow < dwWidthOut; (Row = Row+4), DestRow= DestRow+2)
		{
			U = fgetc(Source) - 128;
			Y1 = fgetc(Source) - 16 ;
			V = fgetc(Source) - 128 ;
			Y2 = fgetc(Source)-16;
			
			Blue = (char)(CLIP(((19070 * Y1) >> 14) + ((33062 * U) >> 14)));
			Green = (char)(CLIP(((19070 * Y1) >> 14) - ((13320 * V) >> 14) - ((6406 * U) >> 14)));
			Red = (char)(CLIP(((19070 * Y1) >> 14) + ((26148 * V) >> 14)));

			
			fputc(Blue,Destination);
			fputc(Green,Destination);
			fputc(Red,Destination);


			Blue = (char)(CLIP(((19070 * Y2) >> 14) + ((33062 * U) >> 14)));
			Green = (char)(CLIP(((19070 * Y2) >> 14) - ((13320 * V) >> 14) - ((6406 * U) >> 14)));
			Red = (char)(CLIP(((19070 * Y2) >> 14) + ((26148 * V) >> 14)));	

			fputc(Blue,Destination);
			fputc(Green,Destination);
			fputc(Red,Destination);
		}
	}	
	fclose(Destination);
	fclose(Source);
	getch();
	return 0;
}

