
//{{BLOCK(bitmap)

//======================================================================
//
//	bitmap, 512x496@4, 
//	+ palette 256 entries, not compressed
//	+ 562 tiles (t|p reduced) not compressed
//	+ regular map (flat), not compressed, 64x62 
//	Total size: 512 + 17984 + 7936 = 26432
//
//	Time-stamp: 2021-03-18, 20:48:19
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.16
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BITMAP_H
#define GRIT_BITMAP_H

#define bitmapTilesLen 17984
extern const unsigned char bitmapTiles[17984];

#define bitmapMapLen 7936
extern const unsigned char bitmapMap[7936];

#define bitmapPalLen 512
extern const unsigned char bitmapPal[512];

#endif // GRIT_BITMAP_H

//}}BLOCK(bitmap)
