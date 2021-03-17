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
    bitmapper(const bitmapper& oldObj);
    bitmapper& operator=(const bitmapper& rhs);

    // public functions
    void setBitmapBits(int, bool);
    bool getBitmapBits(int);
    void toggleBits(int);
    void debugPrintBits();

};


#endif // BITMAPPER