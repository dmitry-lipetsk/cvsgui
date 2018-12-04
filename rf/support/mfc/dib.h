#ifndef DIB_H
#define DIB_H

#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')
/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)   ((i+31)/32*4)
#define WIDTHBYTES16(i)   ((i+15)/16*2)
#define WIDTHBYTES8(i)   ((i+7)/8)

/* Pour les Bitmaps, surchage de Alloc() et utilisation
 * de l'allocateur Windows GlobalAlloc() pour afficher des
 * bitmaps > 64 Ko sous Windows 3.1.
 */

#define BM_ALLOC(nb, size) GlobalAllocPtr( GHND, (nb)*(size) )
#define BM_FREE(ptr) GlobalFreePtr(ptr)

#define BFT_BITMAP 0x4d42   /* 'BM' */

/*****************************************************/
/* make new sizeof structs to cover dword alignment  */
/*****************************************************/

#define SIZEOF_BITMAPFILEHEADER_PACKED  (   \
    sizeof(WORD) +      /* bfType      */   \
    sizeof(DWORD) +     /* bfSize      */   \
    sizeof(WORD) +      /* bfReserved1 */   \
    sizeof(WORD) +      /* bfReserved2 */   \
    sizeof(DWORD))      /* bfOffBits   */

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
VOID DibCopyPalette(HBITMAP , HBITMAP );
WORD PaletteSize (VOID FAR *);
WORD DibNumColors (VOID FAR *);
WORD DibGetPaletteSize (VOID FAR *);
VOID WriteMapFileHeaderandConvertFromDwordAlignToPacked(HFILE, LPBITMAPFILEHEADER);
HANDLE ReadDIBFile(HANDLE);
HANDLE ReadDIBRsrc(HINSTANCE, HRSRC);
HBITMAP LoadDIB(LPSTR);
BOOL SaveDIB(HBITMAP, LPSTR);
HBITMAP CreateDIB(DWORD, DWORD, WORD, HPALETTE);
HBITMAP CopyScreenToBitmap(HWND, LPRECT);
HBITMAP DIBToBitmap(HBITMAP);
HANDLE DDBtoDIB(HBITMAP, WORD, HPALETTE);
BOOL SaveBmpFromDDB(HDC, HBITMAP, PSTR);

#ifdef __cplusplus
}
#endif

#endif /* DIB_H */
