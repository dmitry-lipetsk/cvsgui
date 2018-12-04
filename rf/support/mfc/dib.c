/*************************************************************************
/* This is a 'best of' choice of DIB functions taken from Microsoft
/* examples files (examples wincap32 : file.c, dibutil.c ...)
/*************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include "dib.h"


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibNumColors(VOID FAR * pv)                                *
 *                                                                          *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at   *
 *               the BitCount filed in the info block.                      *
 *                                                                          *
 *  RETURNS    : The number of colors in the DIB.                           *
 *               if bitmap is in 8 bit mode but use only 4 colors           *
 *               function can return 4 (number of color really used)        *
 *               Use DibGetPaletteSize instead...                           *
 *                                                                          *
 ****************************************************************************/
WORD DibNumColors (VOID FAR * pv)
{
    INT                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER)){
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;
        bits = lpbi->biBitCount;
    }
    else
        bits = lpbc->bcBitCount;

    switch (bits){
        case 1:
                return 2;
        case 4:
                return 16;
        case 8:
                return 256;
        default:
                /* A 24 bitcount DIB has no color table */
                return 0;
    }
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibGetPaletteSize(VOID FAR * pv)                           *
 *                                                                          *
 *  PURPOSE    : Return the bitmap encoding type (8 bit, 24 bit...          *
 *               the BitCount filed in the info block.                      *
 *                                                                          *
 *  RETURNS    : the encoding type of the bitmap.                           *
 *                                                                          *
 ****************************************************************************/
WORD DibGetPaletteSize (VOID FAR * pv)
{
    INT                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
        bits = lpbi->biBitCount;
    else
        bits = lpbc->bcBitCount;
	
	return bits;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  DibCopyPalette(HBITMAP src, HBITMAP dst)                  *
 *                                                                          *
 *  PURPOSE    :  Copy the palette from src to dst if possible              *                                                                          *
 *                                                                          *
 ****************************************************************************/
VOID
DibCopyPalette(HBITMAP dst, HBITMAP src)
{
	LPBITMAPINFO src_header, dst_header;
	RGBQUAD *src_rgb, *dst_rgb;
	WORD colorMode;

	src_header = (LPBITMAPINFO) GlobalLock((HGLOBAL)src);
	dst_header = (LPBITMAPINFO) GlobalLock((HGLOBAL)dst);

	src_rgb = (RGBQUAD FAR *)((LPSTR)src_header + (WORD)src_header->bmiHeader.biSize);
	dst_rgb = (RGBQUAD FAR *)((LPSTR)dst_header + (WORD)dst_header->bmiHeader.biSize);

	colorMode = DibGetPaletteSize(src_header);
	switch(colorMode)
	{
		case 4:
		case 8:
			memcpy(dst_rgb, src_rgb,PaletteSize(&src_header->bmiHeader));
			break;
	}

	GlobalUnlock((HGLOBAL)src);
	GlobalUnlock((HGLOBAL)dst);
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  PaletteSize(VOID FAR * pv)                                *
 *                                                                          *
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block  *
 *                is of the BITMAPCOREHEADER type, the number of colors is  *
 *                multiplied by 3 to give the palette size, otherwise the   *
 *                number of colors is multiplied by 4.                                                          *
 *                                                                          *
 *  RETURNS    :  Palette size in number of bytes.                          *
 *                                                                          *
 ****************************************************************************/
WORD PaletteSize (VOID FAR * pv)
{
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
        return (WORD)(NumColors * sizeof(RGBQUAD));
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : WriteMapFileHeaderandConvertFromDwordAlignToPacked(HFILE fh, LPBITMAPFILEHEADER pbf)
 *                                                                          *
 *  PURPOSE    : write header structure (which NOT packed) and write it PACKED
 *                                                                          *
 *  RETURNS    : VOID
 *                                                                          *
 ****************************************************************************/

VOID WriteMapFileHeaderandConvertFromDwordAlignToPacked(HFILE fh, LPBITMAPFILEHEADER pbf)
{

        /* write bfType*/
    _lwrite(fh, (LPSTR)&pbf->bfType, (UINT)sizeof (WORD));
        /* now pass over extra word, and only write next 3 DWORDS!*/
        _lwrite(fh, (LPSTR)&pbf->bfSize, sizeof(DWORD) * 3);
}

/*************************************************************************
 *
 * CreateDIB()
 *
 * Parameters:
 *
 * DWORD dwWidth    - Width for new bitmap, in pixels
 * DWORD dwHeight   - Height for new bitmap 
 * WORD  wBitCount  - Bit Count for new DIB (1, 4, 8, or 24)
 *
 * Return Value:
 *
 * HDIB             - Handle to new DIB
 *
 * Description:
 *
 * This function allocates memory for and initializes a new DIB by
 * filling in the BITMAPINFOHEADER, allocating memory for the color
 * table, and allocating memory for the bitmap bits.  As with all
 * HDIBs, the header, colortable and bits are all in one contiguous
 * memory block.  This function is similar to the CreateBitmap() 
 * Windows API.
 *
 * The colortable and bitmap bits are left uninitialized (zeroed) in the
 * returned HDIB.
 *
 *
 * History:   Date      Author              Reason
 *            3/20/92   Mark Bader          Created
 *
 ************************************************************************/

HBITMAP CreateDIB(DWORD dwWidth, DWORD dwHeight, WORD wBitCount, HPALETTE hPal)
{
    BITMAPINFOHEADER    bi;             /*  bitmap header */
    LPBITMAPINFOHEADER  lpbi;           /*  pointer to BITMAPINFOHEADER */
    DWORD               dwLen;          /*  size of memory block */
    HBITMAP             hDIB;
    DWORD               dwBytesPerLine; /*  Number of bytes per scanline */


    /*  Make sure bits per pixel is valid */

    if (wBitCount <= 1)
        wBitCount = 1;
    else if (wBitCount <= 4)
        wBitCount = 4;
    else if (wBitCount <= 8)
        wBitCount = 8;
    else if (wBitCount <= 24)
        wBitCount = 24;
    else
        wBitCount = 4;  /*  set default value to 4 if parameter is bogus */

    /*  initialize BITMAPINFOHEADER */

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = dwWidth;         /*  fill in width from parameter */
    bi.biHeight = dwHeight;       /*  fill in height from parameter */
    bi.biPlanes = 1;              /*  must be 1 */
    bi.biBitCount = wBitCount;    /*  from parameter */
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;           /*  0's here mean "default" */
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    /*  calculate size of memory block required to store the DIB.  This */
    /*  block should be big enough to hold the BITMAPINFOHEADER, the color */
    /*  table, and the bits */

    dwBytesPerLine = WIDTHBYTES(wBitCount * dwWidth);
    dwLen = bi.biSize + PaletteSize((LPSTR)&bi) + (dwBytesPerLine * dwHeight);

    /*  alloc memory block to store our bitmap */

    hDIB = GlobalAlloc(GHND, dwLen);

    /*  major bummer if we couldn't get memory block */

    if (!hDIB)
        return NULL;

    /*  lock memory and get pointer to it */

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    /*  use our bitmap info structure to fill in first part of */
    /*  our DIB with the BITMAPINFOHEADER */

    *lpbi = bi;

	/*
	 * Copy the palette in the new created image
	 */
	if (wBitCount == 8)
	{
		PALETTEENTRY pal[256];
		RGBQUAD *pRgb;
		int i;

	 	GetPaletteEntries(hPal, 0, 256, pal);

		pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);

		/* Fill in the palette entries from the DIB color table and
		 * create a logical color palette.
		 */
		for (i = 0; i < 256; i++)
		{
			pRgb[i].rgbRed = pal[i].peRed;
			pRgb[i].rgbGreen = pal[i].peGreen;
			pRgb[i].rgbBlue = pal[i].peBlue;
		}
	}

    // Since we don't know what the colortable and bits should contain,
    // just leave these blank.  Unlock the DIB and return the HDIB.

    GlobalUnlock(hDIB);

    //return handle to the DIB

    return hDIB;
}

/*************************************************************************
 *
 * Function:  ReadDIBFile (int)
 *
 *  Purpose:  Reads in the specified DIB file into a global chunk of
 *            memory.
 *
 *  Returns:  A handle to a dib (hDIB) if successful.
 *            NULL if an error occurs.
 *
 * Comments:  BITMAPFILEHEADER is stripped off of the DIB.  Everything
 *            from the end of the BITMAPFILEHEADER structure on is
 *            returned in the global memory handle.
 *
 *
 * NOTE: The DIB API were not written to handle OS/2 DIBs, so this
 * function will reject any file that is not a Windows DIB.
 *
 * History:   Date      Author       Reason
 *            9/15/91   Mark Bader   Based on DIBVIEW
 *            6/25/92   Mark Bader   Added check for OS/2 DIB
 *            7/21/92   Mark Bader   Added code to deal with bfOffBits
 *                                     field in BITMAPFILEHEADER      
 *            9/11/92   Mark Bader   Fixed Realloc Code to free original mem
 *            1/25/94   Hung Nguyen  Changed file APIs to Win32 APIs
 *
 *************************************************************************/

HANDLE ReadDIBFile(HANDLE hFile)
{
    BITMAPFILEHEADER    bmfHeader;
    DWORD               dwBitsSize;
    UINT                nNumColors;   /*  Number of colors in table */
    HANDLE              hDIB;        
    HANDLE              hDIBtmp;      /*  Used for GlobalRealloc() //MPB */
    LPBITMAPINFOHEADER  lpbi;
    DWORD               offBits;
    DWORD               dwRead;

    /*  get length of DIB in bytes for use when reading */

    dwBitsSize = GetFileSize(hFile, NULL);

    /*  Allocate memory for header & color table. We'll enlarge this */
    /*  memory as needed. */

    hDIB = GlobalAlloc(GMEM_MOVEABLE, (DWORD)(sizeof(BITMAPINFOHEADER) +
            256 * sizeof(RGBQUAD)));

    if (!hDIB)
        return NULL;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    if (!lpbi) 
    {
        GlobalFree(hDIB);
        return NULL;
    }

    /*  read the BITMAPFILEHEADER from our file */

    if (!ReadFile(hFile, (LPSTR)&bmfHeader, sizeof (BITMAPFILEHEADER),
            &dwRead, NULL))
        goto ErrExit;

    if (sizeof (BITMAPFILEHEADER) != dwRead)
        goto ErrExit;

    if (bmfHeader.bfType != 0x4d42)  /*  'BM' */
        goto ErrExit;

    /*  read the BITMAPINFOHEADER */

    if (!ReadFile(hFile, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER), &dwRead,
            NULL))
        goto ErrExit;

    if (sizeof(BITMAPINFOHEADER) != dwRead)
        goto ErrExit;

    /*  Check to see that it's a Windows DIB -- an OS/2 DIB would cause */
    /*  strange problems with the rest of the DIB API since the fields */
    /*  in the header are different and the color table entries are */
    /*  smaller. */
    /*  */
    /*  If it's not a Windows DIB (e.g. if biSize is wrong), return NULL. */

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        goto ErrExit;

    /*  Now determine the size of the color table and read it.  Since the */
    /*  bitmap bits are offset in the file by bfOffBits, we need to do some */
    /*  special processing here to make sure the bits directly follow */
    /*  the color table (because that's the format we are susposed to pass */
    /*  back) */

    if (!(nNumColors = (UINT)lpbi->biClrUsed))
    {
        /*  no color table for 24-bit, default size otherwise */

        if (lpbi->biBitCount != 24)
            nNumColors = 1 << lpbi->biBitCount; /*  standard size table */
    }

    /*  fill in some default values if they are zero */

    if (lpbi->biClrUsed == 0)
        lpbi->biClrUsed = nNumColors;

    if (lpbi->biSizeImage == 0)
    {
        lpbi->biSizeImage = ((((lpbi->biWidth * (DWORD)lpbi->biBitCount) +
                31) & ~31) >> 3) * lpbi->biHeight;
    }

    /*  get a proper-sized buffer for header, color table and bits */

    GlobalUnlock(hDIB);
    hDIBtmp = GlobalReAlloc(hDIB, lpbi->biSize + nNumColors *
            sizeof(RGBQUAD) + lpbi->biSizeImage, 0);

    if (!hDIBtmp) /*  can't resize buffer for loading */
        goto ErrExitNoUnlock; /* MPB */
    else
        hDIB = hDIBtmp;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    /*  read the color table */

    ReadFile (hFile, (LPSTR)(lpbi) + lpbi->biSize,
            nNumColors * sizeof(RGBQUAD), &dwRead, NULL);

    /*  offset to the bits from start of DIB header */

    offBits = lpbi->biSize + nNumColors * sizeof(RGBQUAD);

    /*  If the bfOffBits field is non-zero, then the bits might *not* be */
    /*  directly following the color table in the file.  Use the value in */
    /*  bfOffBits to seek the bits. */

    if (bmfHeader.bfOffBits != 0L)
        SetFilePointer(hFile, bmfHeader.bfOffBits, NULL, FILE_BEGIN);

    if (ReadFile(hFile, (LPSTR)lpbi + offBits, lpbi->biSizeImage, &dwRead,
            NULL))
        goto OKExit;


ErrExit:
    GlobalUnlock(hDIB);    

ErrExitNoUnlock:    
    GlobalFree(hDIB);
    return NULL;

OKExit:
    GlobalUnlock(hDIB);
    return hDIB;
}

/*************************************************************************
 *
 * LoadDIB()
 *
 * Loads the specified DIB from a file, allocates memory for it,
 * and reads the disk file into the memory.
 *
 *
 * Parameters:
 *
 * LPSTR lpFileName - specifies the file to load a DIB from
 *
 * Returns: A handle to a DIB, or NULL if unsuccessful.
 *
 * NOTE: The DIB API were not written to handle OS/2 DIBs; This
 * function will reject any file that is not a Windows DIB.
 *
 * History:   Date      Author       Reason
 *            9/15/91   Mark Bader   Based on DIBVIEW
 *
 *************************************************************************/

HBITMAP LoadDIB(LPSTR lpFileName)
{
    HBITMAP     hDIB;
    HANDLE      hFile;

    if ((hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
            NULL)) != INVALID_HANDLE_VALUE)
    {
        hDIB = ReadDIBFile(hFile);
        CloseHandle(hFile);
        return hDIB;
    }
    else
        return NULL;
}

/*************************************************************************
 *
 * SaveBmpFromDDB()
 *
 * Saves the specified DDB into the specified file name on disk.
 *
 *************************************************************************/

BOOL 
SaveBmpFromDDB(HDC hDC, HBITMAP hBmp, PSTR pszFileName)
{
    int         hFile;
    OFSTRUCT    ofReOpenBuff;
    HBITMAP     hTmpBmp, hBmpOld;
    BOOL        bSuccess;
    BITMAPFILEHEADER    bfh;
    PBITMAPINFO pbmi;
    PBYTE       pBits;
    BITMAPINFO  bmi;
    PBYTE pjTmp, pjTmpBmi;
    ULONG sizBMI;


    bSuccess = TRUE;
#if 0
    if (ghPal) {
        SelectPalette(hDC, ghPal, FALSE);
        RealizePalette(hDC);
    }
#endif
    if (!hBmp) {
        return FALSE;
    }

    //
    // Let the graphics engine to retrieve the dimension of the bitmap for us
    // GetDIBits uses the size to determine if it's BITMAPCOREINFO or BITMAPINFO
    // if BitCount != 0, color table will be retrieved
    //
    bmi.bmiHeader.biSize = 0x28;              // GDI need this to work
    bmi.bmiHeader.biBitCount = 0;             // don't get the color table
    if ((GetDIBits(hDC, hBmp, 0, 0, (LPSTR)NULL, &bmi, DIB_RGB_COLORS)) == 0) {
        return FALSE;
    }

    //
    // Now that we know the size of the image, alloc enough memory to retrieve
    // the actual bits
    //
    if ((pBits = (PBYTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                bmi.bmiHeader.biSizeImage)) == NULL) {
        return FALSE;
    }

    //
    // Note: 24 bits per pixel has no color table.  So, we don't have to
    // allocate memory for retrieving that.  Otherwise, we do.
    //
    pbmi = &bmi;                                      // assume no color table

    switch (bmi.bmiHeader.biBitCount) {
        case 24:                                      // has color table
            sizBMI = sizeof(BITMAPINFOHEADER);
            break;
        case 16:
        case 32:
            sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(DWORD)*3;
            break;
        default:
            sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*(1<<bmi.bmiHeader.biBitCount);
            break;

    }

    //
    // Allocate memory for color table if it is not 24bpp...
    //
    if (sizBMI != sizeof(BITMAPINFOHEADER)) {
        ULONG       sizTmp;
        //
        // I need more memory for the color table
        //
        if ((pbmi = (PBITMAPINFO)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizBMI )) == NULL) {
            bSuccess = FALSE;
            goto ErrExit1;
        }
        //
        // Now that we've a bigger chunk of memory, let's copy the Bitmap
        // info header data over
        //
        pjTmp = (PBYTE)pbmi;
        pjTmpBmi = (PBYTE)&bmi;
        sizTmp = sizeof(BITMAPINFOHEADER);

        while(sizTmp--)
        {
            *(((PBYTE)pjTmp)++) = *((pjTmpBmi)++);
        }
    }

    //
    // Let's open the file and get ready for writing
    //
    if ((hFile = OpenFile(pszFileName, (LPOFSTRUCT)&ofReOpenBuff,
                 OF_CREATE | OF_WRITE)) == -1) {
        goto ErrExit2;
    }

    //
    // But first, fill in the info for the BitmapFileHeader
    //
    bfh.bfType = 0x4D42;                            // 'BM'
    bfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizBMI+
        pbmi->bmiHeader.biSizeImage;
    bfh.bfReserved1 =
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizBMI;

    //
    // Write out the file header now
    //
    if (_lwrite(hFile, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER)) == -1) {
        bSuccess = FALSE;
        goto ErrExit3;
    }

    //
    // Bitmap can't be selected into a DC when calling GetDIBits
    // Assume that the hDC is the DC where the bitmap would have been selected
    // if indeed it has been selected
    //
    if (hTmpBmp = CreateCompatibleBitmap(hDC, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight)) {
        hBmpOld = SelectObject(hDC, hTmpBmp);
        if ((GetDIBits(hDC, hBmp, 0, pbmi->bmiHeader.biHeight, (LPSTR)pBits, pbmi, DIB_RGB_COLORS))==0){
            bSuccess = FALSE;
            goto ErrExit4;
        }
    } else {
        bSuccess = FALSE;
        goto ErrExit3;
    }

    //
    // Now write out the BitmapInfoHeader and color table, if any
    //
    if (_lwrite(hFile, (LPSTR)pbmi, sizBMI) == -1) {
        bSuccess = FALSE;
        goto ErrExit4;
    }

    //
    // write the bits also
    //
    if (_lwrite(hFile, (LPSTR)pBits, pbmi->bmiHeader.biSizeImage) == -1) {
        bSuccess = FALSE;
        goto ErrExit4;
    }


ErrExit4:
    SelectObject(hDC, hBmpOld);
    DeleteObject(hTmpBmp);
ErrExit3:
    _lclose(hFile);
ErrExit2:
    GlobalFree(pbmi);
ErrExit1:
    GlobalFree(pBits);
    return bSuccess;
}

/*************************************************************************
 *
 * SaveDIB()
 *
 * Saves the specified DIB into the specified file name on disk.  No
 * error checking is done, so if the file already exists, it will be
 * written over.
 *
 * Parameters:
 *
 * HDIB hDib - Handle to the dib to save
 *
 * LPSTR lpFileName - pointer to full pathname to save DIB under
 *
 * Return value: 0 if successful, or one of:
 *        ERR_INVALIDHANDLE
 *        ERR_OPEN
 *        ERR_LOCK
 *
 * History:
 *
 * NOTE: The DIB API were not written to handle OS/2 DIBs, so this
 * function will not save a file if it is not a Windows DIB.
 *
 * History:   Date      Author       Reason
 *            9/15/91   Mark Bader   Taken from DIBVIEW (which was taken
 *                                      from SHOWDIB)
 *            1/30/92   Mark Bader   Fixed problem of writing too many 
 *                                      bytes to the file
 *            6/24/92   Mark Bader   Added check for OS/2 DIB
 *            2/1/94    Hung Nguyen  Changed file APIs to Win32 APIs
 *
 *************************************************************************/

BOOL SaveDIB(HBITMAP hDib, LPSTR lpFileName)
{
    BITMAPFILEHEADER    bmfHdr;     /*  Header for Bitmap file */
    LPBITMAPINFOHEADER  lpBI;       /*  Pointer to DIB info structure */
    HANDLE              fh;         /*  file handle for opened file */
    DWORD               dwDIBSize;
    DWORD               dwWritten;

    if (!hDib)
        return FALSE;

    fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;

    /*  Get a pointer to the DIB memory, the first of which contains */
    /*  a BITMAPINFO structure */

    lpBI = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    if (!lpBI)
    {
        CloseHandle(fh);
        return FALSE;
    }

    /*  Check to see if we're dealing with an OS/2 DIB.  If so, don't */
    /*  save it because our functions aren't written to deal with these */
    /*  DIBs. */

    if (lpBI->biSize != sizeof(BITMAPINFOHEADER))
    {
        GlobalUnlock(hDib);
        CloseHandle(fh);
        return FALSE;
    }

    /*  Fill in the fields of the file header */

    /*  Fill in file type (first 2 bytes must be "BM" for a bitmap) */

    bmfHdr.bfType = DIB_HEADER_MARKER;  /*  "BM" */

    /*  Calculating the size of the DIB is a bit tricky (if we want to */
    /*  do it right).  The easiest way to do this is to call GlobalSize() */
    /*  on our global handle, but since the size of our global memory may have */
    /*  been padded a few bytes, we may end up writing out a few too */
    /*  many bytes to the file (which may cause problems with some apps, */
    /*  like HC 3.0). */
    /*  */
    /*  So, instead let's calculate the size manually. */
    /*  */
    /*  To do this, find size of header plus size of color table.  Since the */
    /*  first DWORD in both BITMAPINFOHEADER and BITMAPCOREHEADER conains */
    /*  the size of the structure, let's use this. */

    /*  Partial Calculation */

    dwDIBSize = *(LPDWORD)lpBI + PaletteSize((LPSTR)lpBI);  

    /*  Now calculate the size of the image */

    /*  It's an RLE bitmap, we can't calculate size, so trust the biSizeImage */
    /*  field */

    if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
        dwDIBSize += lpBI->biSizeImage;
    else
    {
        DWORD dwBmBitsSize;  /*  Size of Bitmap Bits only */

        /*  It's not RLE, so size is Width (DWORD aligned) * Height */

        dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) *
                lpBI->biHeight;

        dwDIBSize += dwBmBitsSize;

        /*  Now, since we have calculated the correct size, why don't we */
        /*  fill in the biSizeImage field (this will fix any .BMP files which  */
        /*  have this field incorrect). */

        lpBI->biSizeImage = dwBmBitsSize;
    }


    /*  Calculate the file size by adding the DIB size to sizeof(BITMAPFILEHEADER) */
                   
    bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;

    /*  Now, calculate the offset the actual bitmap bits will be in */
    /*  the file -- It's the Bitmap file header plus the DIB header, */
    /*  plus the size of the color table. */
    
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize +
            PaletteSize((LPSTR)lpBI);

    /*  Write the file header */

    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

    /*  Write the DIB header and the bits -- use local version of */
    /*  MyWrite, so we can write more than 32767 bytes of data */
    
    WriteFile(fh, (LPSTR)lpBI, dwDIBSize, &dwWritten, NULL);

    GlobalUnlock(hDib);
    CloseHandle(fh);

    if (dwWritten == 0)
        return FALSE; /*  oops, something happened in the write */
    else
        return TRUE; /*  Success code */
}


/*************************************************************************
/* The following functions were adapted from Microsoft examples sources
/*************************************************************************/

/*************************************************************************
 *
 * CopyScreenToBitmap()
 *
 * Parameter:
 *
 * LPRECT lpRect    - specifies the window
 *
 * Return Value:
 *
 * HDIB             - identifies the device-dependent bitmap
 *
 * Description:
 *
 * This function copies the specified part of the screen to a device-
 * dependent bitmap.
 *
 * History:   Date      Author             Reason
 *            9/15/91   Patrick Schreiber  Created
 *            9/25/91   Patrick Schreiber  Added header and comments
 *
 ************************************************************************/

HBITMAP CopyScreenToBitmap(HWND win, LPRECT lpRect)
{
    HDC         hScrDC, hMemDC;         /*  screen DC and memory DC */
    HBITMAP     hBitmap, hOldBitmap;    /*  handles to deice-dependent bitmaps */
    int         nX, nY, nX2, nY2;       /*  coordinates of rectangle to grab */
    int         nWidth, nHeight;        /*  DIB width and height */
    int         xScrn, yScrn;           /*  screen resolution */
	/*  check for an empty rectangle */

    if (IsRectEmpty(lpRect))
      return NULL;

    /*  create a DC for the screen and create */
    /*  a memory DC compatible to screen DC */
    
    /* hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL); */
    /* hMemDC = CreateCompatibleDC(hScrDC); */
    hScrDC = GetDC(win);
    hMemDC = CreateCompatibleDC (hScrDC);	
	/*  get points of rectangle to grab */

    nX = lpRect->left;
    nY = lpRect->top;		    
    nX2 = lpRect->right;
    nY2 = lpRect->bottom;

    /*  get screen resolution */

    xScrn = GetDeviceCaps(hScrDC, HORZRES);
    yScrn = GetDeviceCaps(hScrDC, VERTRES);

    /* make sure bitmap rectangle is visible */

    if (nX < 0)
        nX = 0;
    if (nY < 0)
        nY = 0;
    if (nX2 > xScrn)
        nX2 = xScrn;
    if (nY2 > yScrn)
        nY2 = yScrn;

    nWidth = nX2 - nX;
    nHeight = nY2 - nY;

    // create a bitmap compatible with the screen DC
    hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

    // select new bitmap into memory DC
    hOldBitmap = SelectObject(hMemDC, hBitmap);

    // bitblt screen DC to memory DC
    BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

    // select old bitmap back into memory DC and get handle to
    // bitmap of the screen
    
    hBitmap = SelectObject(hMemDC, hOldBitmap);

    // clean up

    ReleaseDC(win, hScrDC);
    DeleteDC(hMemDC);

	// return handle to the bitmap

    return hBitmap;
}


HBITMAP
DIBToBitmap(hDIB)
	HBITMAP hDIB;
{
    LPSTR       lpDIBHdr, lpDIBBits;  // pointer to DIB header, pointer to DIB bits
    HBITMAP     hBitmap;            // handle to device-dependent bitmap
    HDC         hDC;                    // handle to DC
    
    if (!hDIB)
        return NULL;

    /* lock memory block and get a pointer to it */
    lpDIBHdr = GlobalLock(hDIB);
    lpDIBBits = lpDIBHdr + *(LPDWORD)lpDIBHdr + PaletteSize(lpDIBHdr);

    /* get a DC */
    hDC = GetDC(NULL);
    if (!hDC)
    {
        GlobalUnlock(hDIB);
        return NULL;
    }

    /* create bitmap from DIB info. and bits */
    hBitmap = CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)lpDIBHdr, CBM_INIT,
            lpDIBBits, (LPBITMAPINFO)lpDIBHdr, DIB_RGB_COLORS);

    
    ReleaseDC(NULL, hDC);
    GlobalUnlock(hDIB);
    return hBitmap;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibFromBitmap()                                            *
 *                                                                          *
 *  PURPOSE    : Will create a global memory block in DIB format that       *
 *               represents the Device-dependent bitmap (DDB) passed in.    *
 *                                                                          *
 *  RETURNS    : A handle to the DIB                                        *
 *                                                                          *
 ****************************************************************************/
HANDLE
DDBtoDIB(hbm, biBits, hPal)
	HBITMAP hbm;
    WORD biBits;
	HPALETTE hPal;
{
    BITMAP               bm;
    BITMAPINFOHEADER     bi;
    BITMAPINFOHEADER FAR *lpbi;
    DWORD                dwLen;
    HANDLE               hdib;
    HANDLE               h;
    HDC                  hdc;
	HPALETTE oldPal;

    if (!hbm)
        return NULL;

    GetObject(hbm,sizeof(bm),(LPSTR)&bm);

    if (biBits == 0)
        biBits =  bm.bmPlanes * bm.bmBitsPixel;

    bi.biSize               = sizeof(BITMAPINFOHEADER);
    bi.biWidth              = bm.bmWidth;
    bi.biHeight             = bm.bmHeight;
    bi.biPlanes             = 1;
    bi.biBitCount           = biBits;
    bi.biCompression        = BI_RGB;
    bi.biSizeImage          = 0;
    bi.biXPelsPerMeter      = 0;
    bi.biYPelsPerMeter      = 0;
    bi.biClrUsed            = 0;
    bi.biClrImportant       = 0;

    dwLen  = bi.biSize + PaletteSize(&bi);

    hdc = GetDC(NULL);
    oldPal = SelectPalette(hdc, hPal, FALSE);
	RealizePalette(hdc);

    hdib = GlobalAlloc(GHND,dwLen);

    if (!hdib)
	{
        SelectPalette(hdc, oldPal, FALSE);
        ReleaseDC(NULL, hdc);
        return NULL;
    }

    lpbi = (VOID FAR *)GlobalLock(hdib);

    *lpbi = bi;

    /*  call GetDIBits with a NULL lpBits param, so it will calculate the
     *  biSizeImage field for us
     */
    GetDIBits(hdc, hbm, 0L, (DWORD)bi.biHeight,
        (LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

    bi = *lpbi;
    GlobalUnlock(hdib);

    /* If the driver did not fill in the biSizeImage field, make one up */
    if (bi.biSizeImage == 0)
        bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;

    /*  realloc the buffer big enough to hold all the bits */
    dwLen = bi.biSize + PaletteSize(&bi) + bi.biSizeImage;

    if (h = GlobalReAlloc(hdib,dwLen,0))
        hdib = h;
    else
	{
        GlobalFree(hdib);
        hdib = NULL;

        SelectPalette(hdc, oldPal, FALSE);
        ReleaseDC(NULL,hdc);
        return hdib;
    }

    /*  call GetDIBits with a NON-NULL lpBits param, and actualy get the
     *  bits this time
     */
    lpbi = (VOID FAR *)GlobalLock(hdib);

    if (GetDIBits( hdc,
                   hbm,
                   0L,
                   (DWORD)bi.biHeight,
                   (LPBYTE)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi),
                   (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS) == 0)
	{
         GlobalUnlock(hdib);
         hdib = NULL;
         SelectPalette(hdc, oldPal, FALSE);
         ReleaseDC(NULL,hdc);
         return NULL;
    }

    bi = *lpbi;
    GlobalUnlock(hdib);

    SelectPalette(hdc, oldPal, FALSE);
    ReleaseDC(NULL,hdc);
    return hdib;
}

/*************************************************************************
 *
 * Function:  ReadDIBRsrc (int)
 *
 *  Purpose:  Reads in the specified DIB file into a global chunk of
 *            memory.
 *
 *  Returns:  A handle to a dib (hDIB) if successful.
 *            NULL if an error occurs.
 *
 * Comments:  BITMAPFILEHEADER is stripped off of the DIB.  Everything
 *            from the end of the BITMAPFILEHEADER structure on is
 *            returned in the global memory handle.
 *
 *
 * NOTE: The DIB API were not written to handle OS/2 DIBs, so this
 * function will reject any file that is not a Windows DIB.
 *
 * History:   Date      Author       Reason
 *            9/15/91   Mark Bader   Based on DIBVIEW
 *            6/25/92   Mark Bader   Added check for OS/2 DIB
 *            7/21/92   Mark Bader   Added code to deal with bfOffBits
 *                                     field in BITMAPFILEHEADER      
 *            9/11/92   Mark Bader   Fixed Realloc Code to free original mem
 *            1/25/94   Hung Nguyen  Changed file APIs to Win32 APIs
 *
 *************************************************************************/

HANDLE ReadDIBRsrc(HINSTANCE hInst, HRSRC hRsrc )
{
    BITMAPFILEHEADER    bmfHeader;
    DWORD               dwBitsSize;
    UINT                nNumColors;   /*  Number of colors in table */
    HANDLE              hDIB;        
    HANDLE              hDIBtmp;      /*  Used for GlobalRealloc() //MPB */
    LPBITMAPINFOHEADER  lpbi;
    DWORD               offBits;
	LPSTR               rsrcptr, beginrsrc;
	HGLOBAL             hg;

    /*  get length of DIB in bytes for use when reading */

	hg = LoadResource(hInst, hRsrc);
	if(hg == NULL)
		return NULL;

	beginrsrc = rsrcptr = LockResource(hg);
	dwBitsSize = SizeofResource(hInst, hRsrc);

    /*  Allocate memory for header & color table. We'll enlarge this */
    /*  memory as needed. */

    hDIB = GlobalAlloc(GMEM_MOVEABLE, (DWORD)(sizeof(BITMAPINFOHEADER) +
            256 * sizeof(RGBQUAD)));

    if (!hDIB)
        goto ErrExitRsrc;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    if (!lpbi) 
    {
        GlobalFree(hDIB);
        goto ErrExitRsrc;
    }

    /*  read the BITMAPFILEHEADER from our file */

    memcpy(&bmfHeader, rsrcptr, sizeof (BITMAPFILEHEADER));
	rsrcptr += sizeof (BITMAPFILEHEADER);

    if (bmfHeader.bfType != 0x4d42)  /*  'BM' */
        goto ErrExit;

    /*  read the BITMAPINFOHEADER */

    memcpy(lpbi, rsrcptr, sizeof(BITMAPINFOHEADER));
	rsrcptr += sizeof (BITMAPINFOHEADER);

    /*  Check to see that it's a Windows DIB -- an OS/2 DIB would cause */
    /*  strange problems with the rest of the DIB API since the fields */
    /*  in the header are different and the color table entries are */
    /*  smaller. */
    /*  */
    /*  If it's not a Windows DIB (e.g. if biSize is wrong), return NULL. */

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        goto ErrExit;

    /*  Now determine the size of the color table and read it.  Since the */
    /*  bitmap bits are offset in the file by bfOffBits, we need to do some */
    /*  special processing here to make sure the bits directly follow */
    /*  the color table (because that's the format we are susposed to pass */
    /*  back) */

    if (!(nNumColors = (UINT)lpbi->biClrUsed))
    {
        /*  no color table for 24-bit, default size otherwise */

        if (lpbi->biBitCount != 24)
            nNumColors = 1 << lpbi->biBitCount; /*  standard size table */
    }

    /*  fill in some default values if they are zero */

    if (lpbi->biClrUsed == 0)
        lpbi->biClrUsed = nNumColors;

    if (lpbi->biSizeImage == 0)
    {
        lpbi->biSizeImage = ((((lpbi->biWidth * (DWORD)lpbi->biBitCount) +
                31) & ~31) >> 3) * lpbi->biHeight;
    }

    /*  get a proper-sized buffer for header, color table and bits */

    GlobalUnlock(hDIB);
    hDIBtmp = GlobalReAlloc(hDIB, lpbi->biSize + nNumColors *
            sizeof(RGBQUAD) + lpbi->biSizeImage, 0);

    if (!hDIBtmp) /*  can't resize buffer for loading */
        goto ErrExitNoUnlock; /* MPB */
    else
        hDIB = hDIBtmp;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    /*  read the color table */

	memcpy((LPSTR)(lpbi) + lpbi->biSize, rsrcptr, nNumColors * sizeof(RGBQUAD));
	rsrcptr += nNumColors * sizeof(RGBQUAD);

    /*  offset to the bits from start of DIB header */
    offBits = lpbi->biSize + nNumColors * sizeof(RGBQUAD);

    /*  If the bfOffBits field is non-zero, then the bits might *not* be */
    /*  directly following the color table in the file.  Use the value in */
    /*  bfOffBits to seek the bits. */

    if (bmfHeader.bfOffBits != 0L)
		rsrcptr = beginrsrc + bmfHeader.bfOffBits;

    memcpy((LPSTR)lpbi + offBits, rsrcptr, lpbi->biSizeImage);
	goto OKExit;

ErrExit:
    GlobalUnlock(hDIB);    

ErrExitNoUnlock:    
    GlobalFree(hDIB);
ErrExitRsrc:   
	FreeResource(hg);
    return NULL;

OKExit:
	FreeResource(hg);
    GlobalUnlock(hDIB);
    return hDIB;
}
