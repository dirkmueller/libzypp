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
    /** \relates IPMutex The underlying mutex file.
     *
     * The underlying mutex file defaults to \c /var/run/zypp/common.lck
     * provided the user has read-write access to it, otherwise
     * \c /var/tmp/zypp-<USER>/common.lck is used.
     */
    Pathname IPMutexCommonLockFile( const Pathname & sysroot_r )
    {
#warning IMPLEMNT user lockfile
      Pathname path( sysroot_r / "/var/run/zypp/common.lck" );
      return path;
    }

  } // namespace
  ///////////////////////////////////////////////////////////////////

  IPMutex::IPMutex()
  : base::InterProcessMutex( IPMutexCommonLockFile( "/" ) )
  {}

  IPMutex::IPMutex( const Pathname & sysroot_r )
  : base::InterProcessMutex( IPMutexCommonLockFile( sysroot_r ) )
  {}

} // namespace zypp
///////////////////////////////////////////////////////////////////
