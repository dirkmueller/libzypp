
#include <sys/wait.h>

#include <iostream>
#include <boost/test/auto_unit_test.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

#include "zypp/TmpPath.h"
#include "zypp/base/LogTools.h"
#include "zypp/base/LogControl.h"
#include "zypp/base/Exception.h"
#include "zypp/base/InterProcessMutex.h"

using boost::unit_test::test_suite;
using boost::unit_test::test_case;
using namespace boost::unit_test;

using namespace std;
using namespace zypp;
using zypp::base::InterProcessMutex;

static filesystem::TmpFile tmpFile;
static Pathname mutexPath( tmpFile.path() );

/** Fork to check the extern visible state of the lockfile. */
int lockStatus()
{
  int pid = fork();
  if ( pid < 0 )
  {
    ERR << "lockStatus fork failed" << endl;
    return 98;
  }
  else if ( pid == 0 )
  {
    // child:
    zypp::base::LogControl::TmpLineWriter shutUp;
    boost::interprocess::file_lock qmutex( mutexPath.c_str() );
    if ( qmutex.try_lock() )
    {
      qmutex.unlock();
      exit( 0 );
    }
    else if ( qmutex.try_lock_sharable() )
    {
      qmutex.unlock_sharable();
      exit( 1 );
    }
    // else
    exit( 2 );
  }
  else
  {
    // parent:
    int ret;
    int status = 0;
    do
    {
      ret = waitpid( pid, &status, 0 );
    }
    while ( ret == -1 && errno == EINTR );

    if ( WIFEXITED( status ) )
    {
      _MIL("___MTX___") << "lockStatus " << WEXITSTATUS( status ) << endl;
      return WEXITSTATUS( status );
    }
    _ERR("___MTX___") << "lockStatus failed" << endl;
    return 99;
  }
}


BOOST_AUTO_TEST_CASE(basic_file_lock_behavior)
{
  // Test basic behavior of boost::file_lock regarding exceptions
  // thrown (or not thrown) on certain command combinations.
  //
  // E.g. lock exclusively then unlock shared -> no exception
  //
  typedef boost::interprocess::file_lock lock_type;
  lock_type mutex( mutexPath.c_str() );

  // Normal sequence
  mutex.lock();
  mutex.unlock();

  mutex.lock_sharable();
  mutex.unlock_sharable();

  // mixed lock unlock (should work as unlock == unlock_sharable)
  mutex.lock();
  mutex.unlock_sharable();

  mutex.lock_sharable();
  mutex.unlock();

  // double unlock
  mutex.unlock();
  mutex.unlock();
  mutex.unlock_sharable();
  mutex.unlock_sharable();

  // double unlock
  mutex.lock();
  mutex.lock();
  mutex.lock_sharable();
  mutex.lock_sharable();

  //
  mutex.unlock();
  mutex.lock_sharable();
  mutex.lock_sharable();
  boost::interprocess::scoped_lock<lock_type> a;
  boost::interprocess::scoped_lock<lock_type> b;
  BOOST_CHECK_THROW( b.lock(), boost::interprocess::lock_exception );
}

BOOST_AUTO_TEST_CASE(basic_mutex)
{
  // Basic operations switch to the requested mutex state. This differs from
  // InterProcessMutex::{SharableLock,ScopedLock} where requesting e.g. a
  // SharableLock is also fulfilled by staying in EXCLUSIVE_LOCK state.
  InterProcessMutex mutex( mutexPath );
  BOOST_CHECK_EQUAL( mutex.state(), InterProcessMutex::UNLOCKED );

#define SWITCH_STATE(CMD,STATE )	\
  mutex.CMD(); BOOST_CHECK_EQUAL( mutex.state(), InterProcessMutex::STATE )

  SWITCH_STATE( lock_sharable,	SHARED_LOCK );
  SWITCH_STATE( lock,		EXCLUSIVE_LOCK );
  SWITCH_STATE( unlock_sharable,UNLOCKED );	//<  unlock_sharable == unlock

  SWITCH_STATE( lock,		EXCLUSIVE_LOCK );
  SWITCH_STATE( lock_sharable,	SHARED_LOCK );
  SWITCH_STATE( unlock,		UNLOCKED );	//<  unlock_sharable == unlock
}

