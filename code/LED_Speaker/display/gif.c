#include "gif.h"

// TODO - implenment and use
static int FreeScreenBuffer(GifRowType *ScreenBuffer) {
  for (i = 0; i < GifFile->SHeight; i++) {
    free(ScreenBuffer[i]);
  }
  (void)free(ScreenBuffer);
  return -1;
}

int gif_2_rgb(char *FileName, uint8_t *Buffer, int FrameSize)
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
    int offset;
    GifRowType GifRow;
    GifColorType *ColorMapEntry;
    uint8_t *BufferP;

    if ((GifFile = DGifOpenFileName(FileName, &Error)) == NULL) {
      fprintf(stderr, "Can't oppen File\n");
      return -1;
    }

    if (GifFile->SHeight == 0 || GifFile->SWidth == 0) {
      fprintf(stderr, "Image of width or height 0\n");
      return -1;
    }

    /*
     * Allocate the screen as vector of column of rows. Note this
     * screen is device independent - it's the screen defined by the
     * GIF file parameters.
     */
    if ((ScreenBuffer = (GifRowType *)malloc(GifFile->SHeight * sizeof(GifRowType))) == NULL) {
      GIF_MESSAGE("Failed to allocate memory required, aborted.");
      return -1;
    }

    Size = GifFile->SWidth * sizeof(GifPixelType);/* Size in bytes one row.*/
    if ((ScreenBuffer[0] = (GifRowType) malloc(Size)) == NULL) { /* First row */
      GIF_MESSAGE("Failed to allocate memory required, aborted.");
      free(ScreenBuffer);
      return -1;
    }

    for (i = 0; i < GifFile->SWidth; i++) {  /* Set its color to BackGround. */
      ScreenBuffer[0][i] = GifFile->SBackGroundColor;
    }

    for (i = 1; i < GifFile->SHeight; i++) {
	/* Allocate the other rows, and set their color to background too: */
      if ((ScreenBuffer[i] = (GifRowType) malloc(Size)) == NULL) {
	GIF_MESSAGE("MEMORY LEAK WILL NOW OCCUR: Failed to allocate memory required, aborted.");
	// TODO - If fail, free memeory from previous malloc
	return -1;
      }

      memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
    }

    /* Scan the content of the GIF file and load the image(s) in: */
    do {
      if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
	fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	return FreeScreenBuffer(ScreenBuffer);
      }

      switch (RecordType) {
      case IMAGE_DESC_RECORD_TYPE:
	if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
	  fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	  return FreeScreenBuffer(ScreenBuffer);
	}
	Row = GifFile->Image.Top; /* Image Position relative to Screen. */
	Col = GifFile->Image.Left;
	Width = GifFile->Image.Width;
	Height = GifFile->Image.Height;
	//	fprintf(stderr,"\n: Image %d at (%d, %d) [%dx%d]:     ", ++ImageNum, Col, Row, Width, Height);
	if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
	    GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) {
	  fprintf(stderr, "ERROR Image %d is not confined to screen dimension, aborted.\n",ImageNum);
	  return FreeScreenBuffer(ScreenBuffer);
	}
	if (GifFile->Image.Interlace) {
	  /* Need to perform 4 passes on the images: */
	  for (Count = i = 0; i < 4; i++)
	    for (j = Row + InterlacedOffset[i]; j < Row + Height; j += InterlacedJumps[i]) {
	      if (DGifGetLine(GifFile, &ScreenBuffer[j][Col], Width) == GIF_ERROR) {
		fprintf(stderr,"ERROR: %d\n",GifFile->Error);
		return FreeScreenBuffer(ScreenBuffer);
	      }
	    }
	}
	else {
	  for (i = 0; i < Height; i++) {
	    if (DGifGetLine(GifFile, &ScreenBuffer[Row++][Col], Width) == GIF_ERROR) {
	      fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	      return FreeScreenBuffer(ScreenBuffer);
	    }
	  }
	}

	/* Lets dump it - set the global variables required and do it: */
	ColorMap = (GifFile->Image.ColorMap ? GifFile->Image.ColorMap : GifFile->SColorMap);
	if (ColorMap == NULL) {
	  fprintf(stderr, "ERROR Gif Image does not have a colormap\n");
	  return FreeScreenBuffer(ScreenBuffer);
	}

	/* check that the background color isn't garbage (SF bug #87) */
	if (GifFile->SBackGroundColor < 0 || GifFile->SBackGroundColor >= ColorMap->ColorCount) {
	  fprintf(stderr, "ERROR Background color out of range for colormap\n");
	  return FreeScreenBuffer(ScreenBuffer);
	}

	offset = FrameSize * (ImageNum - 1);
	//fprintf(stderr,"SHeight: %d \tSWidth: %d\n", GifFile->SHeight, GifFile->SWidth);

	for (i = 0, BufferP = Buffer + offset; i < GifFile->SHeight; i++) {
	  GifRow = ScreenBuffer[i];
	  for (j = 0; j < GifFile->SWidth; j++) {
	    ColorMapEntry = &ColorMap->Colors[GifRow[j]];
	    *BufferP++ = ColorMapEntry->Red;
	    *BufferP++ = ColorMapEntry->Green;
	    *BufferP++ = ColorMapEntry->Blue;
	  }
	}

	break;
      case EXTENSION_RECORD_TYPE:
	/* Skip any extension blocks in file: */
	if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
	  fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	  return FreeScreenBuffer(ScreenBuffer);
	}
	while (Extension != NULL) {
	  if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) {
	    fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	    return FreeScreenBuffer(ScreenBuffer);
	  }
	}
	break;
      case TERMINATE_RECORD_TYPE:
	break;
      default:		    /* Should be trapped by DGifGetRecordType. */
	break;
      }
    } while (RecordType != TERMINATE_RECORD_TYPE);

    for (i = 0; i < GifFile->SHeight; i++) {
      free(ScreenBuffer[i]);
    }
    (void)free(ScreenBuffer);

    if (DGifCloseFile(GifFile, &Error) == GIF_ERROR) {
      fprintf(stderr,"ERROR: %d\n",GifFile->Error);
      return (-2);
    }

    return ImageNum;
}


int gif_frame_count(char *FileName) {

  // I really don't Like this libarry
  // I rather not scan through like this, but need to to get GIF frame count
  /// what ever, adds a little delay in setup time I guess
  int i, j, Size, Row, Col, Width, Height, ExtCode, Count;
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
    int offset;
    GifRowType GifRow;
    GifColorType *ColorMapEntry;

    if ((GifFile = DGifOpenFileName(FileName, &Error)) == NULL) {
      fprintf(stderr, "Can't oppen File\n");
      return -1;
    }

    if (GifFile->SHeight == 0 || GifFile->SWidth == 0) {
      fprintf(stderr, "Image of width or height 0\n");
      return -1;
    }

    /*
     * Allocate the screen as vector of column of rows. Note this
     * screen is device independent - it's the screen defined by the
     * GIF file parameters.
     */
    if ((ScreenBuffer = (GifRowType *)malloc(GifFile->SHeight * sizeof(GifRowType))) == NULL) {
      GIF_MESSAGE("Failed to allocate memory required, aborted.");
      return -1;
    }

    Size = GifFile->SWidth * sizeof(GifPixelType);/* Size in bytes one row.*/
    if ((ScreenBuffer[0] = (GifRowType) malloc(Size)) == NULL) { /* First row */
      GIF_MESSAGE("Failed to allocate memory required, aborted.");
      free(ScreenBuffer);
      return -1;
    }

    for (i = 0; i < GifFile->SWidth; i++) {  /* Set its color to BackGround. */
      ScreenBuffer[0][i] = GifFile->SBackGroundColor;
    }

    for (i = 1; i < GifFile->SHeight; i++) {
      /* Allocate the other rows, and set their color to background too: */
      if ((ScreenBuffer[i] = (GifRowType) malloc(Size)) == NULL) {
	GIF_MESSAGE("MEMORY LEAK WILL NOW OCCUR: Failed to allocate memory required, aborted.");
	// TODO - If fail, free memeory from previous malloc
	return -1;
      }

      memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
    }

    /* Scan the content of the GIF file and load the image(s) in: */
    do {
      if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
	fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	return FreeScreenBuffer(ScreenBuffer);
      }

      switch (RecordType) {
      case IMAGE_DESC_RECORD_TYPE:
	if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
	  fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	  return FreeScreenBuffer(ScreenBuffer);
	}
	Row = GifFile->Image.Top; /* Image Position relative to Screen. */
	Col = GifFile->Image.Left;
	Width = GifFile->Image.Width;
	Height = GifFile->Image.Height;
	//	fprintf(stderr,"\n: Image %d at (%d, %d) [%dx%d]:     ", ++ImageNum, Col, Row, Width, Height);
	if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
	    GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) {
	  fprintf(stderr, "ERROR Image %d is not confined to screen dimension, aborted.\n",ImageNum);
	  return FreeScreenBuffer(ScreenBuffer);
	}
	if (GifFile->Image.Interlace) {
	  /* Need to perform 4 passes on the images: */
	  for (Count = i = 0; i < 4; i++)
	    for (j = Row + InterlacedOffset[i]; j < Row + Height; j += InterlacedJumps[i]) {
	      if (DGifGetLine(GifFile, &ScreenBuffer[j][Col], Width) == GIF_ERROR) {
		fprintf(stderr,"ERROR: %d\n",GifFile->Error);
		return FreeScreenBuffer(ScreenBuffer);
	      }
	    }
	}
	else {
	  for (i = 0; i < Height; i++) {
	    if (DGifGetLine(GifFile, &ScreenBuffer[Row++][Col], Width) == GIF_ERROR) {
	      fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	      return FreeScreenBuffer(ScreenBuffer);
	    }
	  }
	}

	/* Lets dump it - set the global variables required and do it: */
	ColorMap = (GifFile->Image.ColorMap ? GifFile->Image.ColorMap : GifFile->SColorMap);
	if (ColorMap == NULL) {
	  fprintf(stderr, "ERROR Gif Image does not have a colormap\n");
	  return FreeScreenBuffer(ScreenBuffer);
	}

	/* check that the background color isn't garbage (SF bug #87) */
	if (GifFile->SBackGroundColor < 0 || GifFile->SBackGroundColor >= ColorMap->ColorCount) {
	  fprintf(stderr, "ERROR Background color out of range for colormap\n");
	  return FreeScreenBuffer(ScreenBuffer);
	}

	offset = FrameSize * (ImageNum - 1);
	//fprintf(stderr,"SHeight: %d \tSWidth: %d\n", GifFile->SHeight, GifFile->SWidth);

	break;
      case EXTENSION_RECORD_TYPE:
	/* Skip any extension blocks in file: */
	if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
	  fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	  return FreeScreenBuffer(ScreenBuffer);
	}
	while (Extension != NULL) {
	  if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) {
	    fprintf(stderr,"ERROR: %d\n",GifFile->Error);
	    return FreeScreenBuffer(ScreenBuffer);
	  }
	}
	break;
      case TERMINATE_RECORD_TYPE:
	break;
      default:		    /* Should be trapped by DGifGetRecordType. */
	break;
      }
    } while (RecordType != TERMINATE_RECORD_TYPE);

    for (i = 0; i < GifFile->SHeight; i++) {
      free(ScreenBuffer[i]);
    }
    (void)free(ScreenBuffer);

    if (DGifCloseFile(GifFile, &Error) == GIF_ERROR) {
      fprintf(stderr,"ERROR: %d\n",GifFile->Error);
      return (-2);
    }

    return ImageNum;
}
