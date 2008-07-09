//---------------------------------------------------------- -*- Mode: C++ -*-
// Copyright (C) 2007 Josh Taylor (Kosmix Corporation)
// Created 2007-12-10
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

#ifndef KDI_NET_TABLEMANAGERI_H
#define KDI_NET_TABLEMANAGERI_H

#include <warp/builder.h>
#include <warp/call_queue.h>
#include <kdi/net/TimeoutLocator.h>
#include <kdi/marshal/cell_block_builder.h>
#include <kdi/table.h>
#include <kdi/timestamp.h>
#include <boost/noncopyable.hpp>
#include <Ice/Identity.h>

// Generated header
#include <kdi/net/TableManager.h>

namespace kdi {
namespace net {
namespace details {

    class ScannerI;
    class TableI;
    class TableManagerI;

    typedef IceUtil::Handle<ScannerI> ScannerIPtr;
    typedef IceUtil::Handle<TableI> TableIPtr;
    typedef IceUtil::Handle<TableManagerI> TableManagerIPtr;

} // namespace details
} // namespace net
} // namespace kdi


//----------------------------------------------------------------------------
// ScannerI
//----------------------------------------------------------------------------
class kdi::net::details::ScannerI
    : virtual public kdi::net::details::Scanner
{
    kdi::CellStreamPtr scan;
    warp::Builder builder;
    kdi::marshal::CellBlockBuilder cellBuilder;

    kdi::Timestamp lastAccess;
    TimeoutLocatorPtr locator;

public:
    ScannerI(kdi::CellStreamPtr const & scan,
             TimeoutLocatorPtr const & locator);
    ~ScannerI();

    virtual void getBulk(Ice::ByteSeq & cells, bool & lastBlock,
                         Ice::Current const & cur);

    virtual void close(Ice::Current const & cur);

    kdi::Timestamp const & getLastAccess() const { return lastAccess; }
};


//----------------------------------------------------------------------------
// TableI
//----------------------------------------------------------------------------
class kdi::net::details::TableI
    : virtual public kdi::net::details::Table,
      private boost::noncopyable
{
    kdi::TablePtr table;

    kdi::Timestamp lastAccess;
    TimeoutLocatorPtr locator;
    
public:
    TableI(kdi::TablePtr const & table,
           TimeoutLocatorPtr const & locator);
    ~TableI();

    virtual void applyMutations(Ice::ByteSeq const & cells,
                                Ice::Current const & cur);

    virtual void sync(Ice::Current const & cur);

    virtual ScannerPrx scan(std::string const & predicate,
                            Ice::Current const & cur);

    kdi::Timestamp const & getLastAccess() const { return lastAccess; }
};


//----------------------------------------------------------------------------
// TableManagerI
//----------------------------------------------------------------------------
class kdi::net::details::TableManagerI
    : virtual public kdi::net::details::TableManager,
      private boost::noncopyable
{
    TimeoutLocatorPtr locator;

public:
    explicit TableManagerI(TimeoutLocatorPtr const & locator);
    ~TableManagerI();

    virtual TablePrx openTable(std::string const & path,
                               Ice::Current const & cur);
};


#endif // KDI_NET_TABLEMANAGERI_H