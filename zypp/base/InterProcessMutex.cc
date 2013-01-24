/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/base/InterProcessMutex.cc
 *
*/
#include <iostream>

#include "zypp/base/LogTools.h"
#include "zypp/Pathname.h"

#include "zypp/base/InterProcessMutex.h"

#define TAG DBG << __PRETTY_FUNCTION__ << endl;

///////////////////////////////////////////////////////////////////
namespace zypp
{
  ///////////////////////////////////////////////////////////////////
  namespace base
  {
    InterProcessMutex::InterProcessMutex()
    {}

    InterProcessMutex::InterProcessMutex( const char * name_r )
    : lock_type( name_r )
    {}

    InterProcessMutex::InterProcessMutex( const std::string & name_r )
    : lock_type( name_r.c_str() )
    {}

    InterProcessMutex::InterProcessMutex( const Pathname & name_r )
    : lock_type( name_r.c_str() )
    {}

    InterProcessMutex::InterProcessMutex( BOOST_RV_REF(InterProcessMutex) moved_r )
    : lock_type( boost::move(moved_r) )
    {}

    InterProcessMutex & InterProcessMutex::operator=( BOOST_RV_REF(InterProcessMutex) moved_r )
    { lock_type::operator=( boost::move(moved_r) ); return *this; }


    void InterProcessMutex::swap( InterProcessMutex & other_r )
    { lock_type::swap( other_r ); }


    void InterProcessMutex::lock()
    { TAG; return lock_type::lock(); }

    bool InterProcessMutex::try_lock()
    { TAG; return lock_type::try_lock(); }

    bool InterProcessMutex::timed_lock( const boost::posix_time::ptime & abs_time_r )
    { TAG; return lock_type::timed_lock( abs_time_r ); }

    void InterProcessMutex::unlock()
    { TAG; return lock_type::unlock(); }


    void InterProcessMutex::lock_sharable()
    { TAG; return lock_type::lock_sharable(); }

    bool InterProcessMutex::try_lock_sharable()
    { TAG; return lock_type::try_lock_sharable(); }

    bool InterProcessMutex::timed_lock_sharable( const boost::posix_time::ptime & abs_time_r )
    { TAG; return lock_type::timed_lock_sharable( abs_time_r ); }

    void InterProcessMutex::unlock_sharable()
    { TAG; return lock_type::unlock_sharable(); }

  } // namespace base
  ///////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
