/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/base/InterProcessMutex.h
 *
*/

#ifndef ZYPP_BASE_INTER_PROCESS_MUTEX_H
#define ZYPP_BASE_INTER_PROCESS_MUTEX_H

#include <string>

#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "zypp/base/PtrTypes.h"

///////////////////////////////////////////////////////////////////
namespace zypp
{
  class Pathname;

  ///////////////////////////////////////////////////////////////////
  namespace base
  {
    ///////////////////////////////////////////////////////////////////
    /// \class InterProcessMutex
    /// \brief Wrapper around boost::interprocess::file_lock mutex
    ///
    /// Mutex to synchronize filesystem access across different
    /// processes (not threads!) using libzypp. Used e.g. in class
    /// \ref IPMutex.
    ///
    /// The underlying mutex is created per pathname on demand goes out
    /// of scope if the last \ref InterProcessMutex drops it's reference.
    ///
    /// \see http://www.boost.org/doc/html/boost/interprocess/file_lock.html
    ///////////////////////////////////////////////////////////////////
    class InterProcessMutex
    {
    public:
      /** Underlying boost::interprocess::mutex */
      typedef boost::interprocess::file_lock lock_type;

      /** Native exceptions thrown by this class. */
      typedef boost::interprocess::interprocess_exception exception_type;

      /** Aquire and automatically release a shared lock.
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
      typedef boost::interprocess::sharable_lock<InterProcessMutex>	SharableLock;

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
      typedef boost::interprocess::scoped_lock<InterProcessMutex>	ScopedLock;

    public:
      InterProcessMutex();
      InterProcessMutex( const char * name_r );
      InterProcessMutex( const std::string & name_r );
      InterProcessMutex( const Pathname & name_r );

    public:
      /** \name Exclusive locking */
      //@{
	void lock();
	bool try_lock();
	bool timed_lock( const boost::posix_time::ptime & abs_time_r );
	bool wait_lock( unsigned seconds_r )
	{ return timed_lock( wait( seconds_r ) ); }
	void unlock();
      //@}

    public:
      /** \name Sharable locking */
      //@{
	void lock_sharable();
	bool try_lock_sharable();
	bool timed_lock_sharable( const boost::posix_time::ptime & abs_time_r );
	bool wait_lock_sharable( unsigned seconds_r )
	{ return timed_lock_sharable( wait( seconds_r ) ); }
	void unlock_sharable();
      //@}

    public:
      /** Convenience for timed_* methods.
       * \code
       *   InterProcessMutex m;
       *   m.timed_lock_sharable( InterProcessMutex::wait( 5 ) ); // timeout in 5 seconds
       * \endcode
       */
      static boost::posix_time::ptime wait( unsigned seconds_r )
      { return boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds( seconds_r ); }

    public:
      /** Implict conversion to \ref lock_type & */
      operator lock_type &() { return *_mutex; }
      /** \overload const version */
      operator const lock_type &() const { return *_mutex; }

      /** Explicit conversion to \ref lock_type & */
      lock_type & mutex() { return *_mutex; }
      /** \overload const version */
      const lock_type & mutex() const { return *_mutex; }

    private:
      RW_pointer<lock_type> _mutex;
  };
    ///////////////////////////////////////////////////////////////////

  } // namespace base
  ///////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif

