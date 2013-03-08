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

#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "zypp/base/LogTools.h"
#include "zypp/base/NonCopyable.h"
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
      ///////////////////////////////////////////////////////////////////
      /// \class PhoenixMap<PhoenixKeyType, PhoenixValueType>
      /// \brief Map of phoenix-singletons
      ///
      /// A weak_ptr to the created PhoenixValue is stored in the _phoenixMap,
      /// this way subsequent requests for the same PhoenixKey can reuse the
      /// PhoenixValue if it is still in scope. Otherwise the PhoenixValue
      /// is re-created.
      ///
      /// \note PhoenixValueType must be constructible from PhoenixKeyType.
      ///////////////////////////////////////////////////////////////////
      template <class PhoenixKeyType, class PhoenixValueType>
      class PhoenixMap : private base::NonCopyable
      {
      public:
	shared_ptr<PhoenixValueType> get( const PhoenixKeyType & key_r )
	{
	  shared_ptr<PhoenixValueType> ret( _phoenixMap[key_r].lock() ); // weak_ptr lock ;)
	  if ( ! ret )
	  {
	    DBG << "New Phoenix " << key_r << endl;
	    ret.reset( new PhoenixValueType( key_r ) );
	    _phoenixMap[key_r] = ret;
	  }
	  else
	  {
	    DBG << "Reuse Phoenix " << key_r << endl;
	  }
	  return ret;

	}
      private:
	std::map<PhoenixKeyType, weak_ptr<PhoenixValueType> > _phoenixMap;
      };
      ///////////////////////////////////////////////////////////////////

      /** \relates InterProcessMutex */
      typedef PhoenixMap<Pathname, InterProcessMutex::Impl> PhoenixMapType;

      /** \relates InterProcessMutex PhoenixMa of active InterProcessMutexes. */
      inline PhoenixMapType & phoenixMap()
      {
	static PhoenixMapType _phoenixMap;
	return _phoenixMap;
      }

    } // namespace
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    /// \class InterProcessMutex::Impl
    /// \brief \ref InterProcessMutex implementation
    ///////////////////////////////////////////////////////////////////
    class InterProcessMutex::Impl : private base::NonCopyable
    {
    public:
      /** Underlying boost::interprocess::mutex */
      typedef boost::interprocess::file_lock lock_type;

    public:
      Impl( const Pathname & path_r )
      : _mutex( path_r.c_str() )
      , _state( UNLOCKED )
      {}

    public:
      State state() const
      { return _state; }

    public:
      void lock()
      { if ( _state != EXCLUSIVE_LOCK ) 	{ _mutex.lock(); _state = EXCLUSIVE_LOCK; } }

      bool try_lock()
      {
	if ( _state == EXCLUSIVE_LOCK )		return true;
        if ( _mutex.try_lock() )		{ _state = EXCLUSIVE_LOCK; return true; }
        return false;
      }

      bool timed_lock( const boost::posix_time::ptime & abs_time_r )
      {
	if ( _state == EXCLUSIVE_LOCK )		return true;
	if ( _mutex.timed_lock( abs_time_r ) )	{ _state = EXCLUSIVE_LOCK; return true; }
	return false;
      }

      void unlock()	// not bound to EXCLUSIVE_LOCK
      {  if ( _state != UNLOCKED ) 		{ _mutex.unlock(); _state = UNLOCKED; } }

    public:
      void lock_sharable()
      { if ( _state != SHARED_LOCK )		{ _mutex.lock_sharable(); _state = SHARED_LOCK; } }

      bool try_lock_sharable()
      {
	if ( _state == SHARED_LOCK )		return true;
	if ( _mutex.try_lock_sharable() )	{ _state = SHARED_LOCK; return true; }
	return false;
      }

      bool timed_lock_sharable( const boost::posix_time::ptime & abs_time_r )
      {
	if ( _state == SHARED_LOCK )		return true;
	if ( _mutex.timed_lock_sharable( abs_time_r ) ) { _state = SHARED_LOCK; return true; }
	return false;
      }

      void unlock_sharable()	// not bound to SHARED_LOCK
      {  if ( _state != UNLOCKED ) 		{ _mutex.unlock_sharable(); _state = UNLOCKED; } }

    private:
      lock_type	_mutex;
      State	_state;
    };

    /** \relates InterProcessMutex::Impl Stream output */
    std::ostream & operator<<( std::ostream & str, const InterProcessMutex::Impl & obj );

    ///////////////////////////////////////////////////////////////////
    /// \class InterProcessMutex::SharableLock::Impl
    /// \brief \ref InterProcessMutex::SharableLock implementation
    ///////////////////////////////////////////////////////////////////
    class InterProcessMutex::SharableLock::Impl : private base::NonCopyable
    {};

    ///////////////////////////////////////////////////////////////////
    // class InterProcessMutex::SharableLock
    ///////////////////////////////////////////////////////////////////

    InterProcessMutex::SharableLock::SharableLock()
    {}

    InterProcessMutex::SharableLock::SharableLock( InterProcessMutex & mutex_r )
    {}

    InterProcessMutex::SharableLock::SharableLock( InterProcessMutex & mutex_r, defer_lock_type )
    {}

    InterProcessMutex::SharableLock::SharableLock( InterProcessMutex & mutex_r, try_to_lock_type )
    {}

    InterProcessMutex::SharableLock::SharableLock( InterProcessMutex & mutex_r, const boost::posix_time::ptime & abs_time_r )
    {}

    InterProcessMutex::SharableLock::SharableLock( InterProcessMutex & mutex_r, unsigned seconds_r )
    {}

    InterProcessMutex::SharableLock::~SharableLock()
    {}


    void InterProcessMutex::SharableLock::lock()
    {}

    bool InterProcessMutex::SharableLock::try_lock()
    {}

    bool InterProcessMutex::SharableLock::timed_lock( const boost::posix_time::ptime & abs_time_r )
    {}

    void InterProcessMutex::SharableLock::unlock()
    {}


    bool InterProcessMutex::SharableLock::owns() const
    {}

    InterProcessMutex InterProcessMutex::SharableLock::mutex() const
    {}

    ///////////////////////////////////////////////////////////////////
    // class InterProcessMutex
    ///////////////////////////////////////////////////////////////////

    InterProcessMutex::InterProcessMutex()
    {}

    InterProcessMutex::InterProcessMutex( const char * path_r )
    : _pimpl( phoenixMap().get( path_r ) )
    {}

    InterProcessMutex::InterProcessMutex( const std::string & path_r )
    : _pimpl( phoenixMap().get( path_r ) )
    {}

    InterProcessMutex::InterProcessMutex( const Pathname & path_r )
    : _pimpl( phoenixMap().get( path_r ) )
    {}


    InterProcessMutex::State InterProcessMutex::state() const
    { return _pimpl->state(); }

    std::string InterProcessMutex::asString() const
    { return base::asString( state() ); }


    void InterProcessMutex::lock()
    { TAG; return _pimpl->lock(); }

    bool InterProcessMutex::try_lock()
    { TAG; return _pimpl->try_lock(); }

    bool InterProcessMutex::timed_lock( const boost::posix_time::ptime & abs_time_r )
    { TAG; return _pimpl->timed_lock( abs_time_r ); }

    void InterProcessMutex::unlock()
    { TAG; return _pimpl->unlock(); }


    void InterProcessMutex::lock_sharable()
    { TAG; return _pimpl->lock_sharable(); }

    bool InterProcessMutex::try_lock_sharable()
    { TAG; return _pimpl->try_lock_sharable(); }

    bool InterProcessMutex::timed_lock_sharable( const boost::posix_time::ptime & abs_time_r )
    { TAG; return _pimpl->timed_lock_sharable( abs_time_r ); }

    void InterProcessMutex::unlock_sharable()
    { TAG; return _pimpl->unlock_sharable(); }


    ///////////////////////////////////////////////////////////////////
    // enum InterProcessMutex::State
    ///////////////////////////////////////////////////////////////////

    std::string asString( const InterProcessMutex::State obj )
    {
      switch ( obj )
      {
	case InterProcessMutex::UNLOCKED:	return "-nl-";	break;
	case InterProcessMutex::SHARED_LOCK:	return "shar";	break;
	case InterProcessMutex::EXCLUSIVE_LOCK:	return "EXCL";	break;
      }
      INT << "InterProcessMutex::State(?)" << endl;
      return "InterProcessMutex::State(?)";
    }

  } // namespace base
  ///////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
