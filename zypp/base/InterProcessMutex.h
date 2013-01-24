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
    /// Wrapper to ease handling, loging and debuging. Used e.g. in
    /// class \ref IPMutex.
    ///
    /// \see http://www.boost.org/doc/html/boost/interprocess/file_lock.html
    ///////////////////////////////////////////////////////////////////
    class InterProcessMutex : public boost::interprocess::file_lock
    {
    public:
      /** Underlying boost::interprocess::mutex */
      typedef boost::interprocess::file_lock				lock_type;
      /** Native exceptions thrown by this class. */
      typedef boost::interprocess::interprocess_exception		exception_type;

      /** Aquire and automatically release a shared lock. */
      typedef boost::interprocess::sharable_lock<InterProcessMutex>	SharableLock;
      /** Aquire and automatically release an exclusive lock. */
      typedef boost::interprocess::scoped_lock<InterProcessMutex>	ScopedLock;

    public:
      InterProcessMutex();
      InterProcessMutex( const char * name_r );
      InterProcessMutex( const std::string & name_r );
      InterProcessMutex( const Pathname & name_r );

      InterProcessMutex( BOOST_RV_REF(InterProcessMutex) moved_r );
      InterProcessMutex & operator=( BOOST_RV_REF(InterProcessMutex) moved_r );

      void swap( InterProcessMutex & other_r );

    public:
      /** \name Exclusive locking */
      //@{
	void lock();
	bool try_lock();
	bool timed_lock( const boost::posix_time::ptime & abs_time_r );
	void unlock();
      //@}

    public:
      /** \name Sharable locking */
      //@{
	void lock_sharable();
	bool try_lock_sharable();
	bool timed_lock_sharable( const boost::posix_time::ptime & abs_time_r );
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
    };
    ///////////////////////////////////////////////////////////////////

  } // namespace base
  ///////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif

