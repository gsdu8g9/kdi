//---------------------------------------------------------- -*- Mode: C++ -*-
// Copyright (C) 2006 Josh Taylor (Kosmix Corporation)
// Created 2006-01-10
// 
// This file is part of the warp library.
// 
// The warp library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or any later
// version.
// 
// The warp library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//----------------------------------------------------------------------------

#include <warp/apputil.h>
#include <warp/fs.h>
#include <warp/dir.h>

#include <ex/exception.h>

#include <boost/format.hpp>

#include <iostream>
#include <cctype>

using namespace warp;
using namespace ex;
using namespace std;
using boost::format;

//----------------------------------------------------------------------------
// findFilesWithExtension
//----------------------------------------------------------------------------
void warp::findFilesWithExtension(string const & root,
                                  string const & ext,
                                  vector<string> & files)
{
    // Find all files in a heirarchy, in breadth-first, sorted by
    // level order.  For example, the disk-ordered hierarchy:
    //
    //    A/
    //      a.ext
    //      D/
    //        d.ext
    //        c.ext
    //      B/
    //        a.ext
    //        e.ext
    //      g.ext
    //      f.ext
    //
    //  Would be linearized to:
    //
    //    A/a.ext
    //    A/f.ext
    //    A/g.ext
    //    A/B/a.ext
    //    A/B/e.ext
    //    A/D/c.ext
    //    A/D/d.ext
    
    using namespace warp::fs;

    vector<string> dirs;
    size_t firstFile = files.size();

    DirPtr dir = Directory::open(root);

    // Extract files and directories
    string entry;
    while(dir->read(entry))
    {
        if((entry == ".") || (entry == ".."))
            continue;

        string path = resolve(dir->getPath(), entry);

        if(isDirectory(path))
            dirs.push_back(path);
        else if(ext.empty() || extension(path) == ext)
            files.push_back(path);
    }

    // Sort files
    sort(files.begin() + firstFile, files.end());

    // Add directories
    sort(dirs.begin(), dirs.end());
    for(vector<string>::const_iterator it = dirs.begin();
        it != dirs.end(); ++it)
    {
        findFilesWithExtension(*it, ext, files);
    }
}


//----------------------------------------------------------------------------
// findInputFiles
//----------------------------------------------------------------------------
void warp::findInputFiles(vector<string> const & inputArgs,
                          string const & fileExt,
                          vector<string> & result)
{
    using namespace warp::fs;

    for(vector<string>::const_iterator ii = inputArgs.begin();
        ii != inputArgs.end(); ++ii)
    {
        if(isDirectory(*ii))
        {
            findFilesWithExtension(*ii, fileExt, result);
        }
        else if(isFile(*ii))
        {
            if(fileExt.empty() || extension(*ii) == fileExt)
                result.push_back(*ii);
            else
                cerr << format("skipping '%s': does not have extension '%s'")
                    % *ii % fileExt << endl;
        }
        else if(!exists(*ii))
        {
            cerr << format("skipping '%s': does not exist")
                % *ii << endl;
        }
        else
        {
            cerr << format("skipping '%s': unknown file type")
                % *ii << endl;
        }
    }
}


//----------------------------------------------------------------------------
// InputFileGenerator
//----------------------------------------------------------------------------
InputFileGenerator::InputFileGenerator(string_vec_t const & filenames,
                                       bool verbose) :
    filenames(filenames), idx(0), verbose(verbose)
{
}

FilePtr InputFileGenerator::next()
{
    if(idx < filenames.size())
    {
        string const & fn = filenames[idx++];
        if(verbose)
            std::cout << "Reading: " << fn << std::endl;
        FilePtr f(File::input(fn));
        return f;
    }
    else
        throw StopIteration();
}


//----------------------------------------------------------------------------
// SeqFnGenerator
//----------------------------------------------------------------------------
SeqFnGenerator::SeqFnGenerator(string const & base,
                               string const & fnFmt,
                               int startIdx) :
    base(base), fnFmt(fnFmt), idx(startIdx)
{
}

std::string SeqFnGenerator::next()
{
    return fs::resolve(base, (format(fnFmt) % idx++).str());
}
