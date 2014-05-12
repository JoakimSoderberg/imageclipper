/** @file
 *
 * Filesystem functions
 * Currently, this is a boost::filesystem wrapper.
 * If you do not like to use (or install) boost::filesystem, modify here without 
 * changing function interfaces so that the main program is not necessary modified.
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
#ifndef FILESYSTEM_INCLUDED
#define FILESYSTEM_INCLUDED

#include <boost/filesystem.hpp>
#include <vector>
using namespace std;

namespace fs {

    inline void create_directories( const string& path )
    {
        boost::filesystem::path fspath( path );
        boost::filesystem::create_directories( fspath );
    }

    inline bool is_directory( const string& path )
    {
        boost::filesystem::path fspath( path );
        return boost::filesystem::is_directory( fspath );
    }

    inline bool exists( const string& path )
    {
        boost::filesystem::path fspath( path );
        return boost::filesystem::exists( fspath );
    }

    inline string realpath( const string& path )
    {
        boost::filesystem::path fspath( path );
        return fspath.string();
    }

    inline string dirname( const string& path )
    {
        boost::filesystem::path fspath( path );
        return fspath.branch_path().string();
    }

    inline string basename( const string& path )
    {
        boost::filesystem::path fspath( path );
        return fspath.filename().string();
    }

    inline string filename( const string& path )
    {
        boost::filesystem::path fspath( path );
        return boost::filesystem::basename( fspath );
    }

    // @todo: string( ext, 1 ) errors when ext does not exist. 
    inline string extension( const string& path )
    {
        boost::filesystem::path fspath( path );
        return string( boost::filesystem::extension( fspath ), 1 );
    }

    inline string strtolower( const std::string& str )
    {
        std::string ret = str;
        for( std::string::size_type i = 0; i < ret.size(); i++ )
        {
            ret[i] = tolower(ret[i]);
        }
        return ret;
    }

    bool match_extensions( const string& filename, const vector<string>& extensions )
    {
        std::string extension = boost::filesystem::extension( filename );
        extension = fs::strtolower( extension );
        for( std::size_t i = 0; i < extensions.size() - 1; i++ ) {
            if( extension == "." + extensions[i] ) return true;
        }
        return false;
    }

    vector<string> filelist( const string& dirpath, 
                             const vector<string>& extensions,
                             string file_type = "all" )
    {
        vector<string> filelist;
        boost::filesystem::path fs_dirpath( dirpath );
        
        bool list_directory    = ( file_type == "dir" );
        bool list_regular_file = ( file_type == "file" );
        bool list_symlink      = ( file_type == "symlink" );
        bool list_other        = ( !list_directory && !list_regular_file && !list_symlink );
        bool list_all          = ( file_type == "all" );
        
        if( !boost::filesystem::exists( fs_dirpath ) || 
            !boost::filesystem::is_directory( fs_dirpath ) ) {
            return filelist;
        }
        
        boost::filesystem::directory_iterator iter( fs_dirpath ), end_iter;
        for( ; iter != end_iter; ++iter ) {
            boost::filesystem::path filename = iter->path();
            if( match_extensions( filename.string(), extensions ) ) {
                if(list_all) {
                    filelist.push_back( filename.string() );
                } else if(list_regular_file && boost::filesystem::is_regular( filename )) {
                    filelist.push_back( filename.string() );                
                } else if(list_directory && boost::filesystem::is_directory( filename )) {
                    filelist.push_back( filename.string() );
                } else if(list_symlink && boost::filesystem::is_symlink( filename )) {
                    filelist.push_back( filename.string() );
                } else if(list_other && boost::filesystem::is_other( filename )) {
                    filelist.push_back( filename.string() );
                }
            }
        }
        return filelist;
    }

    /*
      vector<boost::filesystem::path> 
      filesystem(const boost::filesystem::path& dirpath, 
      const boost::regex regex = boost::regex(".*"), 
      boost::filesystem::file_type file_type = boost::filesystem::type_unknown)
      {
      vector<boost::filesystem::path> filelist;
      bool list_directory    = (file_type == boost::filesystem::directory_file);
      bool list_regular_file = (file_type == boost::filesystem::regular_file);
      bool list_symlink      = (file_type == boost::filesystem::symlink_file);
      bool list_other        = (!list_directory && !list_regular_file && !list_symlink);
      bool list_all          = (file_type == boost::filesystem::type_unknown); // just for now

      if(!boost::filesystem::exists(dirpath) || !boost::filesystem::is_directory(dirpath)) {
      return filelist;
      }

      boost::filesystem::directory_iterator iter(dirpath), end_iter;
      for(; iter != end_iter; ++iter) {
      boost::filesystem::path filename = iter->path();
      if(boost::regex_match(filename.string(), regex)) {
      if(list_all) {
      filelist.push_back(filename);
      } else if(list_regular_file && boost::filesystem::is_regular(filename)) {
      filelist.push_back(filename);                
      } else if(list_directory && boost::filesystem::is_directory(filename)) {
      filelist.push_back(filename);
      } else if(list_symlink && boost::filesystem::is_symlink(filename)) {
      filelist.push_back(filename);
      } else if(list_other && boost::filesystem::is_other(filename)) {
      filelist.push_back(filename);
      }
      }
      }
      return filelist;
      }
    */
}

#endif
