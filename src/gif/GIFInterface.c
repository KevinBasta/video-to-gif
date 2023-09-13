
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <stdint.h>
#include "main.h"

#include "array.h"
#include "linkedlist.h"
#include "GIFInterface.h"
#include "GIFColorTable.h"
#include "GIFCodeTable.h"

/////////// GIF Canvas Interface ///////////


/**
 * @brief Create GIF canvas record
 * @param canvasWidth  Width in pixels
 * @param canvasHeight Height in pixels
 * @return GIFCanvas or NULL
 */
GIFCanvas *canvasCreate(u16 canvasWidth, u16 canvasHeight) {
    GIFCanvas *newCanvas = calloc(1, sizeof(GIFCanvas));
    if (newCanvas == NULL)
        return NULL;

    newCanvas->canvasWidth = canvasWidth;
    newCanvas->canvasHeight = canvasHeight;

    newCanvas->packedField_GlobalColorTableFlag     = 0;
    newCanvas->packedField_ColorResolution          = 0;
    newCanvas->packedField_SortFlag                 = 0;
    newCanvas->packedField_SizeOfGlobalColorTable   = 0;

    newCanvas->backgroundColorIndex = 0;
    newCanvas->pixelAspectRatio     = 0;

    newCanvas->globalColorTable = NULL;

    newCanvas->frames           = linkedlistInit();
    if (newCanvas->frames == NULL) {
        free(newCanvas);
        return NULL;
    }

    return newCanvas;
}

/**
 * @brief There are two ways to add a global color table to a canvas.
 * 1: Call to give colorTable struct pointer
 * 2: Call to create one and call for each entry insert
 */

// METHOD #1
// Give a colorTable struct pointer directly to the canvas
STATUS_CODE canvasAddGlobalColorTable(GIFCanvas *canvas, colorTable *clrTable) {
    CANVAS_NULL_CHECK(canvas);
    COLOR_TABLE_NULL_CHECK(clrTable);

    if (canvas->globalColorTable != NULL) {
        freeColorTable(canvas->globalColorTable);
    }

    canvas->packedField_GlobalColorTableFlag = 1;
    
    // Color table must be fully populated before this function call
    // or the size of color table packed value may be incorrect
    u32 sizeLog = log2(clrTable->lastIndex);
    canvas->packedField_ColorResolution         = sizeLog;
    canvas->packedField_SizeOfGlobalColorTable  = sizeLog;

    canvas->globalColorTable = clrTable; 

    return OPERATION_SUCCESS;
}


// METHOD #2
// Create color table (colorTable struct) and append individual entries
STATUS_CODE canvasCreateGlobalColorTable(GIFCanvas *canvas) { 
    CANVAS_NULL_CHECK(canvas);

    if (canvas->globalColorTable != NULL) {
        freeColorTable(canvas->globalColorTable);
    }

    colorTable *clrTable = colortableInit();
    COLOR_TABLE_NULL_CHECK(clrTable);

    canvas->packedField_GlobalColorTableFlag = 1;

    u32 sizeLog = log2(clrTable->lastIndex);
    canvas->packedField_ColorResolution         = sizeLog;
    canvas->packedField_SizeOfGlobalColorTable  = sizeLog;
    
    canvas->globalColorTable = clrTable;

    return OPERATION_SUCCESS;
}

STATUS_CODE canvasAddColorToColorTable(GIFCanvas *canvas, u8 red, u8 green, u8 blue) {
    STATUS_CODE status;
    
    CANVAS_NULL_CHECK(canvas);
    COLOR_TABLE_NULL_CHECK(canvas->globalColorTable);

    status = colortableAppendRGB(canvas->globalColorTable, red, green, blue);       
    CHECKSTATUS(status);

    u32 sizeLog = log2(canvas->globalColorTable->lastIndex);
    canvas->packedField_ColorResolution         = sizeLog;
    canvas->packedField_SizeOfGlobalColorTable  = sizeLog;

    return OPERATION_SUCCESS;
}


STATUS_CODE canvasSetBackgroundColorIndex(GIFCanvas *canvas, u8 globalColorTableIndex) {
    CANVAS_NULL_CHECK(canvas);

    if (canvas->globalColorTable == NULL)
        return CANVAS_BACKGROUND_WITHOUT_GLOBAL_CLR_TABLE;

    if (globalColorTableIndex > canvas->globalColorTable->lastIndex)
        return CANVAS_BACKGROUND_INDEX_OUT_OF_BOUNDS;

    canvas->backgroundColorIndex = globalColorTableIndex;
    
    return OPERATION_SUCCESS;
}

STATUS_CODE canvasAddFrame(GIFCanvas *canvas, GIFFrame *frame) {
    CANVAS_NULL_CHECK(canvas);
    FRAME_NULL_CHECK(frame);

    if (canvas->frames == NULL) {
        canvas->frames = linkedlistInit();
    }

    linkedlistAppend(canvas->frames, frame);

    return OPERATION_SUCCESS;
}

void freeCanvas(GIFCanvas *canvas) {
    if (canvas != NULL) {
        if (canvas->globalColorTable != NULL)
            freeColorTable(canvas->globalColorTable);
        
        if (canvas->frames != NULL)
            freeFrameLinkedList(canvas->frames);

        free(canvas);
    }
}





/////////// GIF Frame Interface ///////////


GIFFrame *frameCreate(u16 frameWidth, u16 frameHeight, u16 imageLeftPosition, u16 imageTopPosition) {
    GIFFrame *newFrame = calloc(1, sizeof(GIFFrame));

    newFrame->imageLeftPosition = imageLeftPosition;
    newFrame->imageTopPosition  = imageTopPosition;

    newFrame->imageWidth        = frameWidth;
    newFrame->imageHeight       = frameHeight;

    newFrame->packedField_LocalColorTableFlag       = 0;
    newFrame->packedField_InterlaceFlag             = 0;
    newFrame->packedField_SortFlag                  = 0;
    newFrame->packedField_Reserved                  = 0;
    newFrame->packedField_SizeOfLocalColorTable     = 0;

    newFrame->packedField_GCE_Reserved              = 0;
    newFrame->packedField_GCE_DisposalMethod        = 2;
    newFrame->packedField_GCE_UserInputFlag         = 0;
    newFrame->packedField_GCE_TransparentColorFlag  = 0;

    newFrame->delayTime             = 0;
    newFrame->transparentColorIndex = 0;

    newFrame->localColorTable = NULL;

    newFrame->indexStream = NULL;

    return newFrame;
}

/**
 * @brief There are two ways to add a color table to a frame.
 * 1: Call to give colorTable struct pointer
 * 2: Call to create one and call for each entry insert
 */

// METHOD #1
// Give a colorTable struct pointer directly to the frame
STATUS_CODE frameAddLocalColorTable(GIFFrame *frame, colorTable *clrTable) {
    FRAME_NULL_CHECK(frame);
    COLOR_TABLE_NULL_CHECK(clrTable);

    if (frame->localColorTable != NULL) {
        freeColorTable(frame->localColorTable);
    }

    frame->packedField_LocalColorTableFlag   = 1;

    // Color table must be fully populated before this function call
    // or the size of color table packed value may be incorrect
    u32 sizeLog = log2(clrTable->lastIndex);
    frame->packedField_SizeOfLocalColorTable = sizeLog;
    
    frame->localColorTable = clrTable; 

    return OPERATION_SUCCESS;
}


// METHOD #2
// Create color table (colorTable struct) and append individual entries
STATUS_CODE frameCreateLocalColorTable(GIFFrame *frame) { 
    FRAME_NULL_CHECK(frame);

    if (frame->localColorTable != NULL) {
        freeColorTable(frame->localColorTable);
    }

    colorTable *clrTable = colortableInit();
    COLOR_TABLE_NULL_CHECK(clrTable);

    frame->packedField_LocalColorTableFlag   = 1;
    
    u32 sizeLog = log2(clrTable->lastIndex);
    frame->packedField_SizeOfLocalColorTable   = sizeLog;
    
    frame->localColorTable = clrTable; 

    return OPERATION_SUCCESS;
}

STATUS_CODE frameAddColorToColorTable(GIFFrame *frame, u8 red, u8 green, u8 blue) {
    STATUS_CODE status;
    
    FRAME_NULL_CHECK(frame);
    COLOR_TABLE_NULL_CHECK(frame->localColorTable);

    status = colortableAppendRGB(frame->localColorTable, red, green, blue);       
    CHECKSTATUS(status);

    u32 sizeLog = log2(frame->localColorTable->lastIndex);
    frame->packedField_SizeOfLocalColorTable = sizeLog;

    return OPERATION_SUCCESS;
}

/**
 * @brief There are three ways to add an index stream
 * to a frame.
 * 1: Call to give an array struct pointer
 * 2: Call to give a stack array and it's size
 * 3: Call to create one and call for each entry insert
 */

// METHOD #1
// Give the array struct pointer directly to the frame
STATUS_CODE frameAddIndexStream(GIFFrame *frame, array *indexStream) {
    FRAME_NULL_CHECK(frame);
    ARRAY_NULL_CHECK(indexStream);

    if (frame->indexStream != NULL) {
        freeArray(frame->indexStream);
    }

    frame->indexStream = indexStream;

    return OPERATION_SUCCESS;
}

// METHOD #2
// Give an array pointer and its size to the frame 
STATUS_CODE frameCreateIndexStreamFromArray(GIFFrame *frame, u8 stackArr[], size_t size) {
    FRAME_NULL_CHECK(frame);

    if (frame->indexStream != NULL) {
        freeArray(frame->indexStream);
    }

    frame->indexStream = arrayInitFromStackArray((u8*)stackArr, size);
    ARRAY_NULL_CHECK(frame->indexStream);

    return OPERATION_SUCCESS;
}

// METHOD #3
// Create index stream (array struct) and append individual entries
STATUS_CODE frameCreateIndexStream(GIFFrame *frame, size_t indexStreamSize) {
    FRAME_NULL_CHECK(frame);

    if (frame->indexStream != NULL) {
        freeArray(frame->indexStream);
    }

    frame->indexStream = arrayInit(indexStreamSize);
    ARRAY_NULL_CHECK(frame->indexStream);

    return OPERATION_SUCCESS;
}

STATUS_CODE frameAppendToIndexStream(GIFFrame *frame, u32 item) {
    STATUS_CODE status;
    
    FRAME_NULL_CHECK(frame);
    ARRAY_NULL_CHECK(frame->indexStream);

    status = arrayAppend(frame->indexStream, item);
    CHECKSTATUS(status);

    return OPERATION_SUCCESS;
}


STATUS_CODE frameAddGraphicsControlInfo(GIFFrame *frame, u8 disposalMethod, u16 delayTime) {
    FRAME_NULL_CHECK(frame);

    frame->packedField_GCE_DisposalMethod = disposalMethod;
    frame->delayTime                      = delayTime;

    return OPERATION_SUCCESS;
}

STATUS_CODE frameSetTransparanetColorIndexInColorTable(GIFFrame *frame, u8 transparentColorIndex) {
    FRAME_NULL_CHECK(frame);

    frame->packedField_GCE_TransparentColorFlag = 1;
    frame->transparentColorIndex                = transparentColorIndex;

    return OPERATION_SUCCESS;
}

void freeFrame(GIFFrame *frame) {
    if (frame != NULL) {
        if (frame->localColorTable != NULL)
            freeColorTable(frame->localColorTable);

        if (frame->indexStream != NULL)
            freeArray(frame->indexStream);

        free(frame);
    }
}