//---------------------------------------------------------- -*- Mode: C++ -*-
// Copyright (C) 2007 Josh Taylor (Kosmix Corporation)
// Created 2007-10-08
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

#ifndef KDI_LOCAL_TABLE_TYPES_H
#define KDI_LOCAL_TABLE_TYPES_H

#include <warp/string_data.h>
#include <warp/offset.h>
#include <warp/bloom_filter.h>
#include <kdi/marshal/cell_block.h>

namespace kdi {
namespace local {
namespace disk {

    // Simple structured format.  This can use string pooling like
    // SDStore's StaticTable.  This format is CPU-efficient, but not
    // space-efficient.  It is reasonable for tables that need to be
    // pinned into memory.
    //
    // Table file format:
    //   1+  Record of CellBlock
    //   1   Record of BlockIndex
    //   1   Record of TableInfo
    //   <EOF>
    //
    // The way to read the file is to seek to the TableInfo record at
    // 10+sizeof(TableInfo) bytes before EOF, read that, then find and
    // read the BlockIndex.

    using kdi::marshal::CellKey;
    using kdi::marshal::CellData;
    using kdi::marshal::CellBlock;

    /// Mapping from a beginning CellKey to a CellBlock offset.
    struct IndexEntryV0 {
        CellKey startKey;
        uint64_t blockOffset;  // from beginning of file
    };

    // Richer index format
    struct IndexEntryV1
    {
        uint32_t blockChecksum; // Adler-32
        warp::StringOffset lastRow;
        uint64_t blockOffset;
        int64_t lowestTime;
        int64_t highestTime;
        uint32_t colFamilyMask;
    };

    /// Base BlockIndex type defines record type code
    struct BlockIndex
    {
        enum {
            TYPECODE = WARP_PACK4('C','B','I','x'),
            FLAGS = 0,
            ALIGNMENT = 8,
        };
    };

    /// Index of CellBlock records in the file.
    struct BlockIndexV0 : public BlockIndex
    {
        enum { VERSION = 0 };
        warp::ArrayOffset<IndexEntryV0> blocks;
    };

    // Index of CellBlock records using the new format
    struct BlockIndexV1 : public BlockIndex
    {
        enum { VERSION = 1 };
        warp::ArrayOffset<IndexEntryV1> blocks;
        warp::ArrayOffset<warp::StringOffset> colFamilies;
    };

    // Trailer for a disk table file.
    struct TableInfo
    {
        // TableInfo structure shouldn't change.  It's used to figure
        // out what kind of DiskTable file we have.  The record
        // version tells us what to expect from the rest of the file.

        enum {
            TYPECODE = WARP_PACK4('T','N','f','o'),
            VERSION = 1,        // latest version
            FLAGS = 0,
            ALIGNMENT = 8,
        };

        uint64_t indexOffset;   // from beginning of file

        TableInfo() {}
        explicit TableInfo(uint64_t off) : indexOffset(off) {}
    };

} // namespace disk
} // namespace local
} // namespace kdi

#endif // KDI_LOCAL_TABLE_TYPES_H
