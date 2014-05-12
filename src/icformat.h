/** @file
* 
* Image clipper filename format
*
* The MIT License
* 
* Copyright (c) 2008, Naotoshi Seo <sonots(at)umd.edu>
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#ifndef IC_FORMAT_INCLUDED
#define IC_FORMAT_INCLUDED

#include <stdio.h>
#include <string>

/**
* Convert format to string
*
* @param format The format string
* - \%d => dirname
* - \%i => filename
* - \%e => extension
* - \%x => x
* - \%y => y
* - \%w => width
* - \%h => height
* - \%f => frame number (for video file)
* - \%r => rotation (int degree)
* - \%. => shear deformation in x coordinate
* - \%, => shear deformation in y coordinate
* @param dirname The directoryname
* @param filename The filename excluding file extension
* @param extension The file extension
* @param x
* @param y
* @param width
* @param height
* @param frame
* @param rotation
* @param shear_x
* @param shear_y
* @return string
*/
string icFormat(const string& format, const string& dirname, const string& filename, const string& extension, 
               int x, int y, int width, int height, int frame = 0, int rotation = 0, int shear_x = 0, int shear_y = 0)
{
    string ret = format;
    char tmp[2048];
    char intkeys[] = { 'x', 'y', 'w', 'h', 'f', 'r', '.', ',' };
    int  intvals[] = { x, y, width, height, frame, rotation, shear_x, shear_y };
    int nintkeys = 8;
    char strkeys[] = { 'i', 'e', 'd' };
    std::string strvals[] = { filename, extension, dirname };
    int nstrkeys = 3;
    for(int i = 0; i < nintkeys + nstrkeys; i++) {
        std::string::size_type start = ret.find("%");
        if(start == std::string::npos) break;
        std::string::size_type minstrpos = std::string::npos;
        std::string::size_type minintpos = std::string::npos;
        int minstrkey = INT_MAX; int minintkey = INT_MAX;
        for(int j = 0; j < nstrkeys; j++) {
            std::string::size_type pos = ret.find(strkeys[j], start);
            if(pos < minstrpos) {
                minstrpos = pos;
                minstrkey = j;
            }
        }
        for(int j = 0; j < nintkeys; j++) {
            std::string::size_type pos = ret.find(intkeys[j], start);
            if(pos < minintpos) {
                minintpos = pos;
                minintkey = j;
            }
        }
        if(minstrpos == std::string::npos && minintpos == std::string::npos) break;
        if(minstrpos < minintpos) {
            string format_substr = ret.substr(start, minstrpos - start) + "s";
            std::sprintf(tmp, format_substr.c_str(), strvals[minstrkey].c_str());
            ret.replace(start, minstrpos - start + 1, string(tmp));
        } else {
            string format_substr = ret.substr(start, minintpos - start) + "d";
            std::sprintf(tmp, format_substr.c_str(), intvals[minintkey]);
            ret.replace(start, minintpos - start + 1, string(tmp));
        }
    }
    return ret;
}

#endif
