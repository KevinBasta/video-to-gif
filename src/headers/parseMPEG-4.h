
void readMainBoxes(u8 fileName[], linkedList *list);
void parseChildBoxes(box *parentBox, linkedList *list);
void parseNestedChildBoxes(u8 *boxData, u32 *bytesRead, u32 endIndex, linkedList *list);
box *parseSingleNestedChildBox(u8 *boxData, u32 *bytesRead);
box *getVideTrak(linkedList *moovLL);
