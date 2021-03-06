//---------------------------------------------------------- -*- Mode: C++ -*-
// Copyright (C) 2006 Josh Taylor (Kosmix Corporation)
// Created 2006-04-03
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

#include <warp/tmpfile.h>
#include <warp/stdfile.h>
#include <warp/fs.h>
#include <warp/posixfs.h>
#include <ex/exception.h>

#include <algorithm>
#include <stdlib.h>

using namespace warp;
using namespace ex;
using namespace std;


//----------------------------------------------------------------------------
// TmpFileGen
//----------------------------------------------------------------------------
TmpFileGen::TmpFileGen(string const & rootDir) :
    rootDir(rootDir), madeDir(false)
{
    FsPtr p = Filesystem::get(rootDir);
    if(p != PosixFilesystem::get())
        raise<RuntimeError>("TmpFileGen only works on Posix filesystem");

    if(p->exists(rootDir) && !p->isDirectory(rootDir))
        raise<IOError>("tmp file root is not a directory: %s", rootDir);
}

FilePtr TmpFileGen::next()
{
    if(!madeDir)
    {
        fs::makedirs(rootDir);
        madeDir = true;
    }

    return openTmpFile(rootDir, "tmp-");
}

FilePtr warp::openTmpFile(string const & dir, string const & prefix)
{
    string fn = fs::resolve(dir, prefix + "XXXXXX");
    string p = fs::path(fn);
    int fd = mkstemp(&p[0]);
    if(fd < 0)
        raise<IOError>("mkstemp failed for '%s': %s", p, getStdError());

    // mkstemp modifies filename in place.  Update filename URI.
    fn = fs::replacePath(fn, p);

    FilePtr fp(new StdFile(fd, fn, "wb"));
    return fp;
}
