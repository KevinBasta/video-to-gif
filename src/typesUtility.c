#ifndef MPEG_HEAD
    #include <stdio.h>
    #include <stdlib.h>

    #include "headers/types.h"
    #include "headers/printUtility.h"
    #include "headers/bitUtility.h"
#endif

// search algorithm for stss/sync sample
// returns the preceeding 
unsigned int binarySearch(unsigned int sampleNumber, syncSampleTableEntry **syncSampleTable, unsigned int syncSampleTableEntries) { 
    unsigned int low = 0;
    unsigned int high = syncSampleTableEntries;
    unsigned int mid; 
    
    while (low <= high) { 
        mid = (low + high) / 2;

        if (sampleNumber == syncSampleTable[mid]->number) { 
            return mid;
        } else if (sampleNumber < syncSampleTable[mid]->number) { 
            high = mid - 1;
        } else if (sampleNumber > syncSampleTable[mid]->number) { 
            low = mid + 1;
        }

        //printf("%d %d %d %d %d\n", low, high, mid, sampleNumber, syncSampleTable[mid]->number);
    }

    return low; // since the preceeding key frame is required
}



// free functions

// for tables with no pointer values
// cast param passed using (void**)
void freeTable(void **table) {
    int i = 0;
    while (table[i] != NULL) { 
        free(table[i]);
        i++;
    }
    free(table);
}

void freeDataReferenceTable(dataReferenceTableEntry **dataReferenceTable) { 
    int i = 0;
    while (dataReferenceTable[i] != NULL) { 
        free(dataReferenceTable[i]->type);
        free(dataReferenceTable[i]->version);
        free(dataReferenceTable[i]->flags);
        free(dataReferenceTable[i]->data);
        free(dataReferenceTable[i]);
        i++;
    }

    free(dataReferenceTable);
}

void freeSampleDescription() { 

}