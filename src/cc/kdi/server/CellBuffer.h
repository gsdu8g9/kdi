//---------------------------------------------------------- -*- Mode: C++ -*-
// Copyright (C) 2009 Josh Taylor (Kosmix Corporation)
// Created 2009-02-27
//
// This file is part of KDI.
//
// KDI is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or any later version.
//
// KDI is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//----------------------------------------------------------------------------

#ifndef KDI_SERVER_CELLBUFFER_H
#define KDI_SERVER_CELLBUFFER_H

#include <kdi/server/Fragment.h>
#include <kdi/strref.h>
#include <warp/string_range.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace kdi {
namespace server {

    class CellBuffer;
    typedef boost::shared_ptr<CellBuffer const> CellBufferCPtr;
    
} // namespace server
} // namespace kdi

//----------------------------------------------------------------------------
// CellBuffer
//----------------------------------------------------------------------------
class kdi::server::CellBuffer
    : public kdi::server::Fragment,
      private boost::noncopyable
{
public:
    CellBuffer(strref_t data);

    /// Get the unique rows included in this cell buffer, in
    /// increasing string order.  The rows will be returned as
    /// references into this buffer's memory, so they're only valid
    /// for the lifetime of the buffer.
    void getRows(std::vector<warp::StringRange> & rows) const;
    
    /// Get the approximate amount of memory used by this buffer.
    size_t getDataSize() const { return data.size(); }

public:  // Fragment interface
    size_t nextBlock(ScanPredicate const & pred, size_t minBlock) const;
    std::auto_ptr<FragmentBlock> loadBlock(size_t blockAddr) const;

private:
    std::vector<char> data;
    size_t nCells;

    class Block;
    class BlockReader;
};

#endif // KDI_SERVER_CELLBUFFER_H