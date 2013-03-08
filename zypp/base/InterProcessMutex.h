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

#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "zypp/base/PtrTypes.h"
#include "zypp/base/SafeBool.h"
#include "zypp/base/NonCopyable.h"

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
      /** Native exceptions thrown by this class. */
      typedef boost::interprocess::interprocess_exception exception_type;

      /** Type to indicate to a mutex lock constructor that it must not lock the mutex. */
      struct defer_lock_type {};
      /** An object indicating that lock operation must be defered. */
      static constexpr defer_lock_type defer_lock = defer_lock_type();

      /** Type to indicate to a mutex lock constructor that it must try to lock the mutex. */
      struct try_to_lock_type {};
      /** An object indicating that a try_lock() operation must be executed. */
      static constexpr try_to_lock_type try_to_lock = try_to_lock_type();

      /** Aquire and automatically release a shared lock. */
      class SharableLock;

      /** Aquire and automatically release an exclusive lock. */
      typedef boost::interprocess::scoped_lock<InterProcessMutex>	ScopedLock;

    public:
      /** Default Ctor (use as placeholder only) */
      InterProcessMutex();
      /** Ctor creating mutex for \a path_r  */
      explicit InterProcessMutex( const char * path_r );
      /** \overload */
      explicit InterProcessMutex( const std::string & path_r );
      /** \overload */
      explicit InterProcessMutex( const Pathname & path_r );

    public:
      /** Representing the mutex internal state */
      enum State
      { UNLOCKED, SHARED_LOCK, EXCLUSIVE_LOCK };

      /** Return the mutex internal state */
      State state() const;

      /** String representation of mutex state (for logging) */
      std::string asString() const;

    public:
      /** \name Exclusive locking
       * Basically the native boost api to support
       * \c boost::interprocess::scoped_lock<InterProcessMutex>
       * though \ref SharableLock and \ref ScopedLock are preferred.
       */
      //@{
	void lock();
	bool try_lock();
	bool timed_lock( const boost::posix_time::ptime & abs_time_r );
	bool wait_lock( unsigned seconds_r )
	{ return timed_lock( wait( seconds_r ) ); }
	void unlock();
      //@}

    public:
      /** \name Sharable locking
       * Basically the native boost api to support
       * \c boost::interprocess::sharable_lock<InterProcessMutex>
       * though \ref SharableLock and \ref ScopedLock are preferred.
       */
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
      class Impl;
    private:
      RW_pointer<Impl> _pimpl;
  };
  ///////////////////////////////////////////////////////////////////

  /** \relates InterProcessMutex::State String representation */
  std::string asString( const InterProcessMutex::State obj );

  /** \relates InterProcessMutex::State Stream output */
  inline std::ostream & operator<<( std::ostream & str, const InterProcessMutex::State obj )
  { return str << asString( obj ); }

  /** \relates InterProcessMutex Stream output */
  inline std::ostream & operator<<( std::ostream & str, const InterProcessMutex & obj )
  { return str << obj.asString(); }

  ///////////////////////////////////////////////////////////////////
  /// \class InterProcessMutex::SharableLock
  /// \brief Aquire and automatically release a shared lock.
  /// \ingroup g_RAII
  /// \code
  ///   InterProcessMutex mutex;
  ///
  ///   {
  ///     // construct a sharable lock
  ///     InterProcessMutex::SharableLock slock( mutex );
  ///     ...
  ///   } // shared lock is automatically released
  /// \endcode
  /// \code
  ///   {
  ///     // try to construct a sharable lock
  ///     InterProcessMutex::SharableLock slock( mutex, InterProcessMutex::try_to_lock );
  ///     if ( slock.owns() )
  ///     {
  ///       // obtained a sharable lock
  ///       ...
  ///     }
  ///   } // shared lock is automatically released if obtained
  /// \endcode
  /// \code
  ///   {
  ///     // try to construct a sharable lock within 5 seconds
  ///     InterProcessMutex::SharableLock slock( mutex, 5 );
  ///     if ( slock.owns() )
  ///     {
  ///       // obtained a sharable lock
  ///       ...
  ///     }
  ///   } // shared lock is automatically released if obtained
  /// \endcode
  /// \code
  ///   {
  ///     // may or may not need a sharable lock
  ///     InterProcessMutex::SharableLock slock( mutex, InterProcessMutex::defer_lock );
  ///     // no lock obtained by now
  ///     if ( lock_needed() )
  ///     {
  ///       // obtain a sharable lock:
  ///       slock.lock();
  ///       ...
  ///     }
  ///     // still locked sharable if obtained
  ///   } // shared lock is automatically released if obtained
  /// \endcode
  ///////////////////////////////////////////////////////////////////
  class InterProcessMutex::SharableLock : protected base::SafeBool<SharableLock>, private base::NonCopyable
  {
  public:
    SharableLock();
    explicit SharableLock( InterProcessMutex & mutex_r );
    SharableLock( InterProcessMutex & mutex_r, defer_lock_type );
    SharableLock( InterProcessMutex & mutex_r, try_to_lock_type );
    SharableLock( InterProcessMutex & mutex_r, const boost::posix_time::ptime & abs_time_r );
    SharableLock( InterProcessMutex & mutex_r, unsigned seconds_r );
    ~SharableLock();
  public:
    void lock();
    bool try_lock();
    bool timed_lock( const boost::posix_time::ptime & abs_time_r );
    bool wait_lock( unsigned seconds_r )
    { return timed_lock( wait( seconds_r ) ); }
    void unlock();
    bool owns() const;
    using base::SafeBool<SharableLock>::operator bool_type;
    InterProcessMutex mutex() const;
  private:
    friend SafeBool<SharableLock>::operator bool_type() const;
    bool boolTest() const { return owns(); }
  public:
    class Impl;
  private:
    RW_pointer<Impl> _pimpl;
  };
  ///////////////////////////////////////////////////////////////////

  } // namespace base
  ///////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif

