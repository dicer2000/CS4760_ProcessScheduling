/********************************************
 * bitmapper - Bitmap class
 * This is a special class to create & use
 * bitmaps
 * (c)2021 Brett Huffman
 * 
 * Brett Huffman
 * bitmapper .cpp file for project
 ********************************************/

#include <iostream>
#include bitmapper.h

use namespace std;

// Constructors / Destructors

bitmapper::bitmapper(int nSize)
{
    _size = nSize;
    _usageArray = new unsigned char[nSize];
}

 bitmapper::~bitmapper()
 {
     delete _usageArray [];
 }


void bitmapper::setBitmapByte(int addr, bool value)
{
    if(value)
    {
        // Set the bit at this point in the bitmap
        _usageArray[addr/8] |= (1 << (7 - (addr%8)));
    }
    else
    {
        // Clear the bit
        _usageArray[addr/8] &= ~(1 << (7 - (addr%8)));
    }
}

bool bitmapper::getBitmapByte(int addr)
{
    // returns true or false based on whether value
    // is set to 1 or 0 in bitmap
    return (_usageArray[addr/8] & (1 << (7 - (addr%8))));
}

void bitmapper::toggleByte(int addr)
{
    // Toggle the bit at this point in the bitmap
    _usageArray[addr/8] ^= (1 << (7 - (addr%8)));
}