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
    ///////////////////////////////////////////////////////////////////
    namespace
    {
      typedef std::map<Pathname, weak_ptr<InterProcessMutex::lock_type>> MutexMap;

      inline MutexMap & mutexes()
      {
	static MutexMap _mutexes;
	return _mutexes;
      }

      /** \relates IPMutex Prepare underlying mutex (phoenix-singleton).
       *
       * A weak_ptr to the created mutex is stored in the MutexMap, this way
       * subsequent requests for the same path can reuse the mutex if it is
       * still in scope. Otherwise te mutex is re-created.
       */
      inline shared_ptr<InterProcessMutex::lock_type> getMutex( const Pathname & path_r )
      {
	shared_ptr<InterProcessMutex::lock_type> ret( mutexes()[path_r].lock() ); // weak_ptr lock ;)
	if ( ! ret )
	{
	  DBG << "New IPM " << path_r << endl;
	  ret.reset( new base::InterProcessMutex::lock_type( path_r.c_str() ) );
	  mutexes()[path_r] = ret;
	}
	else
	{
	  DBG << "Reuse IPM " << path_r << endl;
	}
	return ret;
      }

    } // namespace
    ///////////////////////////////////////////////////////////////////

    InterProcessMutex::InterProcessMutex()
    {}

    InterProcessMutex::InterProcessMutex( const char * name_r )
    : _mutex( getMutex( name_r ) )
    {}

    InterProcessMutex::InterProcessMutex( const std::string & name_r )
    : _mutex( getMutex( name_r ) )
    {}

    InterProcessMutex::InterProcessMutex( const Pathname & name_r )
    : _mutex( getMutex( name_r ) )
    {}


    void InterProcessMutex::lock()
    { TAG; return _mutex->lock(); }

    bool InterProcessMutex::try_lock()
    { TAG; return _mutex->try_lock(); }

    bool InterProcessMutex::timed_lock( const boost::posix_time::ptime & abs_time_r )
    { TAG; return _mutex->timed_lock( abs_time_r ); }

    void InterProcessMutex::unlock()
    { TAG; return _mutex->unlock(); }


    void InterProcessMutex::lock_sharable()
    { TAG; return _mutex->lock_sharable(); }

    bool InterProcessMutex::try_lock_sharable()
    { TAG; return _mutex->try_lock_sharable(); }

    bool InterProcessMutex::timed_lock_sharable( const boost::posix_time::ptime & abs_time_r )
    { TAG; return _mutex->timed_lock_sharable( abs_time_r ); }

    void InterProcessMutex::unlock_sharable()
    { TAG; return _mutex->unlock_sharable(); }

  } // namespace base
  ///////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
