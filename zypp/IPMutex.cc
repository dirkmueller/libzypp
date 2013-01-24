/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/IPMutex.cc
 */
#include <iostream>
#include "zypp/base/LogTools.h"

#include "zypp/Pathname.h"
#include "zypp/IPMutex.h"

using std::endl;

///////////////////////////////////////////////////////////////////
namespace zypp
{
  ///////////////////////////////////////////////////////////////////
  namespace
  {
    typedef std::map<Pathname, weak_ptr<base::InterProcessMutex>> MutexMap;

    inline MutexMap & mutexes()
    {
      static MutexMap _mutexes;
      return _mutexes;
    }

    /*
     * A weak_ptr to the created mutex is stored in the MutexMap, this way
     * subsequent requests for the same path can reuse the mutex if it is
     * still in scope. Otherwise te mutex is re-created.
     */
    inline shared_ptr<base::InterProcessMutex> getMutex( const Pathname & path_r )
    {
      shared_ptr<base::InterProcessMutex> ret( mutexes()[path_r].lock() );
      if ( ! ret )
      {
	DBG << "New IPM " << path_r << endl;
	ret.reset( new base::InterProcessMutex( path_r ) );
	mutexes()[path_r] = ret;
      }
      else
      {
	DBG << "Reuse IPM " << path_r << endl;
      }
      return ret;
    }

    /** \relates IPMutex Prepare underlying mutex file (phoenix-singleton).
     *
     * The underlying mutex file defaults to \c /var/run/zypp/common.lck
     * provided the user has read-write access to it, otherwise
     * \c /var/tmp/zypp-<USER>/common.lck is created and used.
     */
    shared_ptr<base::InterProcessMutex> IPMutexCommonLockFile( const Pathname & sysroot_r )
    {
      Pathname path( sysroot_r / "/var/run/zypp/common.lck" );
      return getMutex( path );
    }

  } // namespace
  ///////////////////////////////////////////////////////////////////


  IPMutex::IPMutex()
  : _mutex( IPMutexCommonLockFile( "/" ) )
  {}

  IPMutex::IPMutex( const Pathname & sysroot_r )
  : _mutex( IPMutexCommonLockFile( sysroot_r ) )
  {}

} // namespace zypp
///////////////////////////////////////////////////////////////////
