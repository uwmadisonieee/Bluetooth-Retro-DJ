
/*****************************************************************************

gif2rgb - convert GIF to 24-bit RGB pixel triples or vice-versa

*****************************************************************************/

/***************************************************************************

Toshio Kuratomi had written this in a comment about the rgb2gif code:

  Besides fixing bugs, what's really needed is for someone to work out how to
  calculate a colormap for writing GIFs from rgb sources.  Right now, an rgb
  source that has only two colors (b/w) is being converted into an 8 bit GIF....
  Which is horrendously wasteful without compression.

I (ESR) took this off the main to-do list in 2012 because I don't think
the GIFLIB project actually needs to be in the converters-and-tools business.
Plenty of hackers do that; our job is to supply stable library capability
with our utilities mainly interesting as test tools.

***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#include <gif_lib.h>



#define GIF_MESSAGE(Msg) fprintf(stderr, "\ngiflib: %s\n", Msg)
#define GIF_EXIT(Msg)    { GIF_MESSAGE(Msg); exit(-3); }


/******************************************************************************
 The real screen dumping routine.
******************************************************************************/
static void DumpScreen2RGB(
			   ColorMapObject *ColorMap,
			   GifRowType *ScreenBuffer,
			   int ScreenWidth, int ScreenHeight)
{
    int i, j;
    GifRowType GifRow;
    GifColorType *ColorMapEntry;
    FILE *rgbfp;

       
      if ((rgbfp = fopen("test.rgb", "wb")) == NULL) {
            GIF_EXIT("Can't open input file name.");
      }
      unsigned char *Buffer, *BufferP;

      if ((Buffer = (unsigned char *) malloc(ScreenWidth * 3)) == NULL)
	GIF_EXIT("Failed to allocate memory required, aborted.");
      for (i = 0; i < ScreenHeight; i++) {
	GifRow = ScreenBuffer[i];
	printf("\b\b\b\b%-4d", ScreenHeight - i);
	for (j = 0, BufferP = Buffer; j < ScreenWidth; j++) {
	  ColorMapEntry = &ColorMap->Colors[GifRow[j]];
	  *BufferP++ = ColorMapEntry->Red;
	  *BufferP++ = ColorMapEntry->Green;
	  *BufferP++ = ColorMapEntry->Blue;
	}
	if (fwrite(Buffer, ScreenWidth * 3, 1, rgbfp) != 1)
	  GIF_EXIT("Write to file(s) failed.");
      }

      free((char *) Buffer);
      fclose(rgbfp);
}

static void GIF2RGB(char *FileName)
{
    int	i, j, Size, Row, Col, Width, Height, ExtCode, Count;
    GifRecordType RecordType;
    GifByteType *Extension;
    GifRowType *ScreenBuffer;
    GifFileType *GifFile;
    int
	InterlacedOffset[] = { 0, 4, 2, 1 }, /* The way Interlaced image should. */
	InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */
    int ImageNum = 0;
    ColorMapObject *ColorMap;
    int Error;


    if ((GifFile = DGifOpenFileName(FileName, &Error)) == NULL) {
      fprintf(stderr, "Can't oppen File\n");
      exit(EXIT_FAILURE);
    }
    
    if (GifFile->SHeight == 0 || GifFile->SWidth == 0) {
	fprintf(stderr, "Image of width or height 0\n");
	exit(EXIT_FAILURE);
    }

    /* 
     * Allocate the screen as vector of column of rows. Note this
     * screen is device independent - it's the screen defined by the
     * GIF file parameters.
     */
    if ((ScreenBuffer = (GifRowType *)
	malloc(GifFile->SHeight * sizeof(GifRowType))) == NULL)
	    GIF_EXIT("Failed to allocate memory required, aborted.");

    Size = GifFile->SWidth * sizeof(GifPixelType);/* Size in bytes one row.*/
    if ((ScreenBuffer[0] = (GifRowType) malloc(Size)) == NULL) /* First row. */
	GIF_EXIT("Failed to allocate memory required, aborted.");

    for (i = 0; i < GifFile->SWidth; i++)  /* Set its color to BackGround. */
	ScreenBuffer[0][i] = GifFile->SBackGroundColor;
    for (i = 1; i < GifFile->SHeight; i++) {
	/* Allocate the other rows, and set their color to background too: */
	if ((ScreenBuffer[i] = (GifRowType) malloc(Size)) == NULL)
	    GIF_EXIT("Failed to allocate memory required, aborted.");

	memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
    }

    /* Scan the content of the GIF file and load the image(s) in: */
    do {
	if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
	  printf("ERROR: %s\n",GifFile->Error);
	    exit(EXIT_FAILURE);
	}
	switch (RecordType) {
	    case IMAGE_DESC_RECORD_TYPE:
	      printf("IMAGE_DESC_RECORD_TYPE\n");
	      if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
		printf("ERROR: %s\n",GifFile->Error);
		    exit(EXIT_FAILURE);
		}
		Row = GifFile->Image.Top; /* Image Position relative to Screen. */
		Col = GifFile->Image.Left;
		Width = GifFile->Image.Width;
		Height = GifFile->Image.Height;
		printf("\n: Image %d at (%d, %d) [%dx%d]:     ",
		    ++ImageNum, Col, Row, Width, Height);
		if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
		   GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) {
		    fprintf(stderr, "Image %d is not confined to screen dimension, aborted.\n",ImageNum);
		    exit(EXIT_FAILURE);
		}
		if (GifFile->Image.Interlace) {
		    /* Need to perform 4 passes on the images: */
		    for (Count = i = 0; i < 4; i++)
			for (j = Row + InterlacedOffset[i]; j < Row + Height;
						 j += InterlacedJumps[i]) {
			    printf("\b\b\b\b%-4d", Count++);
			    if (DGifGetLine(GifFile, &ScreenBuffer[j][Col],
				Width) == GIF_ERROR) {
			      printf("ERROR: %s\n",GifFile->Error);
				exit(EXIT_FAILURE);
			    }
			}
		}
		else {
		    for (i = 0; i < Height; i++) {
			printf("\b\b\b\b%-4d", i);
			if (DGifGetLine(GifFile, &ScreenBuffer[Row++][Col],
				Width) == GIF_ERROR) {
			  printf("ERROR: %s\n",GifFile->Error);
			  exit(EXIT_FAILURE);
			}
		    }
		}
		break;
	    case EXTENSION_RECORD_TYPE:
	      printf("EXTENSION TYPE ------------------\n");
		/* Skip any extension blocks in file: */
		if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
		  printf("ERROR: %s\n",GifFile->Error);
		    exit(EXIT_FAILURE);
		}
		while (Extension != NULL) {
		    if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) {
		      printf("ERROR: %s\n",GifFile->Error);
			exit(EXIT_FAILURE);
		    }
		}
		break;
	    case TERMINATE_RECORD_TYPE:
	      printf("TERMINATE TYPE ------------------\n");
		break;
	    default:		    /* Should be trapped by DGifGetRecordType. */
		break;
	}
    } while (RecordType != TERMINATE_RECORD_TYPE);
    
    /* Lets dump it - set the global variables required and do it: */
    ColorMap = (GifFile->Image.ColorMap
		? GifFile->Image.ColorMap
		: GifFile->SColorMap);
    if (ColorMap == NULL) {
        fprintf(stderr, "Gif Image does not have a colormap\n");
        exit(EXIT_FAILURE);
    }

    /* check that the background color isn't garbage (SF bug #87) */
    if (GifFile->SBackGroundColor < 0 || GifFile->SBackGroundColor >= ColorMap->ColorCount) {
        fprintf(stderr, "Background color out of range for colormap\n");
        exit(EXIT_FAILURE);
    }

    DumpScreen2RGB(
		   ColorMap,
		   ScreenBuffer, 
		   GifFile->SWidth, GifFile->SHeight);

    (void)free(ScreenBuffer);


    if (DGifCloseFile(GifFile, &Error) == GIF_ERROR) {
      printf("ERROR: %s\n",GifFile->Error);
      exit(EXIT_FAILURE);
    }

}

/******************************************************************************
* Interpret the command line and scan the given GIF file.
******************************************************************************/
int main(int argc, char *argv[])
{

  GIF2RGB(argv[1]);
  
  return 0;
}

/* end */
