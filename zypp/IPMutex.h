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

#include "zypp/base/PtrTypes.h"
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
  /// The underlying mutex is created on demand goes out of scope if
  /// the last \ref IPMutex drops it's reference. An \ref IPMutex can
  /// be implicitly converted int a \ref base::InterProcessMutex reference.
  ///
  /// The underlying mutex file defaults to \c /var/run/zypp/common.lck
  /// provided the user has read-write access to it, otherwise
  /// \c /var/tmp/zypp-<USER>/common.lck is created and used.
  ///////////////////////////////////////////////////////////////////
  class IPMutex
  {
  public:
    /** Underlying boost::interprocess::mutex */
    typedef base::InterProcessMutex::lock_type			lock_type;
    /** Native exceptions thrown by this class. */
    typedef base::InterProcessMutex::exception_type		exception_type;

    /** Aquire and automatically release a sharable lock.
     * \code
     *   IPMutex mutex;
     *
     *   {
     *     // construct a sharable lock
     *     IPMutex::SharableLock slock( mutex );
     *     ...
     *   } // shared lock is automatically released
     *
     *   {
     *     // try to construct a sharable lock within 5 seconds
     *     IPMutex::SharableLock slock( mutex, IPMutex::wait( 5 ) );
     *     if ( slock.owns() )
     *     {
     *       // obtained a sharable lock
     *       ...
     *     }
     *   } // shared lock is automatically released if obtained
     * \endcode
     */
    typedef base::InterProcessMutex::SharableLock	SharableLock;

    /** Aquire and automatically release an exclusive lock.
     * \code
     *   IPMutex mutex;
     *
     *   {
     *     // construct an exclusive lock
     *     IPMutex::ScopedLock slock( mutex );
     *     ...
     *   } // exclusive lock is automatically released
     *
     *   {
     *     // try to construct an exclusive lock within 5 seconds
     *     IPMutex::ScopedLock lock( mutex, IPMutex::wait( 5 ) );
     *     if ( lock.owns() )
     *     {
     *       // obtained an exclusive lock
     *       ...
     *     }
     *   } // exclusive lock is automatically released if obtained
     * \endcode
     */
    typedef base::InterProcessMutex::ScopedLock	ScopedLock;

  public:
    /** Common mutex to synchronize filesystem access (guess the systems root). */
    IPMutex();

    /** Common mutex to synchronize filesystem access (assume system root at \a sysroot_r). */
    IPMutex( const Pathname & sysroot_r );

  public:
    /** \name Exclusive locking */
    //@{
      void lock()	{ return _mutex->lock(); }
      bool try_lock()	{ return _mutex->try_lock(); }
      bool timed_lock( const boost::posix_time::ptime & abs_time )	{ return _mutex->timed_lock( abs_time ); }
      void unlock()	{ return _mutex->unlock(); }
    //@}

  public:
    /** \name Sharable locking */
    //@{
      void lock_sharable()	{ return _mutex->lock_sharable(); }
      bool try_lock_sharable()	{ return _mutex->try_lock_sharable(); }
      bool timed_lock_sharable( const boost::posix_time::ptime & abs_time )	{ return _mutex->timed_lock_sharable( abs_time ); }
      void unlock_sharable()	{ return _mutex->unlock_sharable(); }
    //@}

  public:
    /** Convenience for timed_* methods.
     * \code
     *   IPMutex m;
     *   m.timed_lock_sharable( IPMutex::wait( 5 ) ); // timeout in 5 seconds
     * \endcode
     */
    static boost::posix_time::ptime wait( unsigned seconds_r ) { return base::InterProcessMutex::wait( seconds_r ); }

  public:
    /** Implict conversion to \ref base::InterProcessMutex & */
    operator base::InterProcessMutex &() { return *_mutex; }
    /** \overload const version */
    operator const base::InterProcessMutex &() const { return *_mutex; }

    /** Explicit conversion to \ref base::InterProcessMutex & */
    base::InterProcessMutex & mutex() { return *_mutex; }
    /** \overload const version */
    const base::InterProcessMutex & mutex() const { return *_mutex; }

  private:
    shared_ptr<base::InterProcessMutex> _mutex;
};
  ///////////////////////////////////////////////////////////////////

} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_IPMUTEX_H
