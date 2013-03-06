/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/IPMutex.h
 */
#ifndef ZYPP_IPMUTEX_H
#define ZYPP_IPMUTEX_H

#include <iosfwd>

#include "zypp/base/InterProcessMutex.h"

///////////////////////////////////////////////////////////////////
namespace zypp
{
  class Pathname;

  ///////////////////////////////////////////////////////////////////
  /// \class IPMutex
  /// \brief Common interprocess mutex
  ///
  /// Commom mutex to synchronize filesystem access across different
  /// processes (not threads!) using libzypp.
  ///
  /// The underlying mutex file defaults to \c /var/run/zypp/common.lck
  /// provided the user has read-write access to it, otherwise
  /// \c /var/tmp/zypp-<USER>/common.lck is created and used.
  ///////////////////////////////////////////////////////////////////
  class IPMutex : public base::InterProcessMutex
  {
  public:
    /** Common mutex to synchronize filesystem access (guess the systems root). */
    IPMutex();

    /** Common mutex to synchronize filesystem access (assume system root at \a sysroot_r). */
    explicit IPMutex( const Pathname & sysroot_r );
  };
  ///////////////////////////////////////////////////////////////////

} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_IPMUTEX_H
