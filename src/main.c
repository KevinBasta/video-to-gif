#ifndef MPEG_HEAD
    #define MPEG_HEAD
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    //#include <string.h>
    #include <math.h>
    #include <time.h>

    #include "headers/types.h"
    #include "headers/typeUtility.h"
    #include "headers/printUtility.h"
    #include "headers/endianUtility.h"
    #include "headers/bitUtility.h"
    #include "headers/linkedList.h"
    #include "headers/AVCUtility.h"

    #include "headers/parseMPEG-4.h"
    #include "headers/decodeMPEG-4.h"
    #include "headers/decodeAVC.h"
    #include "headers/processMPEG-4.h"
#endif


int main(int argc, char **argv) { 
    // get path and video name from user

    // Use parseMPEG-4 interface to get video frame information
    // "pipe" that information into a the gif interface functional style
    
    float startTime = (float)clock()/CLOCKS_PER_SEC;
    linkedList *topBoxesLL = initLinkedList();
    // fopen taken in a relative path from executable location
    readMainBoxes("local_files/op.mp4", topBoxesLL);

    MPEG_Data *videoData = (MPEG_Data*) malloc(sizeof(MPEG_Data));
    videoData->mdatBox = getBoxFromLinkedList(topBoxesLL, "mdat");
    videoData->mdatDataOffsetInFile = getOffsetToBoxLinkedList(topBoxesLL, "mdat") + BOX_HEADER_SIZE;


    printf("%d %d\n", videoData->mdatDataOffsetInFile, videoData->mdatBox->boxSize);
    printNBytes(videoData->mdatBox->boxType, 4, "type: " ,"\n");

    printf("----------TOP LEVEL----------\n");
    printAllBoxesLinkedList(topBoxesLL);
    

    printf("----------MOOV LEVEL----------\n");
    box *moovBox = getBoxFromLinkedList(topBoxesLL, "moov");
    
    linkedList *moovLL = initLinkedList();
    parseChildBoxes(moovBox, moovLL);
    printAllBoxesLinkedList(moovLL);


    printf(">----------MVHD LEVEL----------\n");
    box *mvhdBox = getBoxFromLinkedList(moovLL, "mvhd");
    mvhdParseBox(mvhdBox, videoData);



    printf("----------TRAK LEVEL----------\n");
    box *trakBox = getVideTrak(moovLL);

    linkedList *trakLL = initLinkedList();
    parseChildBoxes(trakBox, trakLL);
    printAllBoxesLinkedList(trakLL);



    
    printf(">----------TKHD LEVEL----------\n");
    box *tkhdBox = getBoxFromLinkedList(trakLL, "tkhd");
    tkhdParseBox(tkhdBox, videoData);


    printf(">----------EDTS LEVEL----------\n");
    box *edtsBox = getBoxFromLinkedList(trakLL, "edts");
    edtsParseBox(edtsBox, videoData);




    printf("----------MDIA LEVEL----------\n");
    box *mdia = getBoxFromLinkedList(trakLL, "mdia");

    linkedList *mdiaLL = initLinkedList();
    parseChildBoxes(mdia, mdiaLL);
    printAllBoxesLinkedList(mdiaLL);




    printf(">----------MDHD LEVEL----------\n");
    box *mdhdBox = getBoxFromLinkedList(mdiaLL, "mdhd");
    mdhdParseBox(mdhdBox, videoData);


    printf(">----------HDLR LEVEL----------\n");
    box *hdlrBox = getBoxFromLinkedList(mdiaLL, "hdlr");
    hdlrParseBox(hdlrBox);





    printf("----------MINF LEVEL----------\n");
    box *minf = getBoxFromLinkedList(mdiaLL, "minf");

    linkedList *minfLL = initLinkedList();
    parseChildBoxes(minf, minfLL);
    printAllBoxesLinkedList(minfLL);



    printf(">----------DINF LEVEL----------\n");
    box *dinfBox = getBoxFromLinkedList(minfLL, "dinf");
    dinfParseBox(dinfBox, videoData);



    printf("----------STBL LEVEL----------\n");
    box *stbl = getBoxFromLinkedList(minfLL, "stbl");

    linkedList *stblLL = initLinkedList();
    parseChildBoxes(stbl, stblLL);
    printAllBoxesLinkedList(stblLL);


    printf(">--------STSD CHILD LEVEL--------\n");
    box *stco = getBoxFromLinkedList(stblLL, "stco");
    stcoParseBox(stco, videoData);
    box *stsz = getBoxFromLinkedList(stblLL, "stsz");
    stszParseBox(stsz, videoData);
    printf("=============sample to chunk===================\n");
    box *stsc = getBoxFromLinkedList(stblLL, "stsc");
    stscParseBox(stsc, videoData);
    printf("=====================================\n");
    box *stss = getBoxFromLinkedList(stblLL, "stss");
    stssParseBox(stss, videoData);
    box *ctts = getBoxFromLinkedList(stblLL, "ctts");
    cttsParseBox(ctts, videoData);
    box *stts = getBoxFromLinkedList(stblLL, "stts");
    sttsParseBox(stts, videoData);
    printf("=====================================\n");
    box *stsd = getBoxFromLinkedList(stblLL, "stsd");
    stsdParseBox(stsd, videoData);
    printf("=====================================\n");
    float endTime = (float)clock()/CLOCKS_PER_SEC;
    float timeElapsed = endTime - startTime;
    printf("DATA PARSING OPERATION elapsed: %f\n", timeElapsed);


    createDisplayTimeToSampleTable(videoData);
    //sampleSearchByRealTime(15, videoData);
    
    
    startTime = (float)clock()/CLOCKS_PER_SEC;
    getVideoDataRangeByMediaTime(15, 20, videoData);
    endTime = (float)clock()/CLOCKS_PER_SEC;
    timeElapsed = endTime - startTime;
    printf("DATA PROCESSING OPERATION elapsed: %f\n", timeElapsed);
    //keyFrameSearch(79, videoData);

    // free every linked list created
    /* freeLinkedList(topBoxesLL, "box");
    freeLinkedList(moovLL, "box");
    freeLinkedList(trakLL, "box");
    freeLinkedList(mdiaLL, "box");
    freeLinkedList(minfLL, "box");
    freeLinkedList(stblLL, "box"); */
    printf("end of script\n");
    return 0;
}