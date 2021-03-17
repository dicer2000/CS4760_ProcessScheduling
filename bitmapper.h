/********************************************
 * bitmapper - Bitmap class
 * This is a special class to create & use
 * bitmaps
 * (c)2021 Brett Huffman
 * 
 * Brett Huffman
 * bitmapper .h file for project
 ********************************************/
#ifndef BITMAPPER
#define BITMAPPER


class bitmapper
{
    private:
        unsigned char* _usageArray;
        int _size;
        

    public:

    bitmapper(int nSize);
    ~bitmapper();

    // public functions
    void setBitmapByte(uint, bool);
    bool getBitmapByte(int);
    void toggleByte(int);


};


#endif // BITMAPPER