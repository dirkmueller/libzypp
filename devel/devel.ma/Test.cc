#include "Tools.h"
#include <sys/wait.h>
#include <zypp/IPMutex.h>

int lockStatus()
{
#if ( 1 )
  #define FOR fork()
  #define EXI exit
#else
  #define FOR 0
  #define EXI return
#endif
  int pid = FOR;
  if ( pid < 0 )
  {
    ERR << "lockStatus failed" << endl;
    return 98;
  }
  else if ( pid == 0 )
  {
    // child:
    zypp::base::LogControl::TmpLineWriter shutUp;
    base::InterProcessMutex qmutex( "/var/run/zypp/common.lck" );
    if ( qmutex.try_lock() )
    {
      qmutex.unlock();
      EXI( 0 );
    }
    else if ( qmutex.try_lock_sharable() )
    {
      qmutex.unlock_sharable();
      EXI( 1 );
    }
    else
    {
      EXI( 2 );
    }
    EXI( 13 );
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

std::ostream & operator<<( std::ostream & str, const IPMutex::SharableLock & obj )
{
  return str << obj.owns() << '(' << lockStatus() << ')';
}
std::ostream & operator<<( std::ostream & str, const IPMutex::ScopedLock & obj )
{
  return str << obj.owns() << '(' << lockStatus() << ')';
}

#define LTAG(X) MIL << X << " " << #X << endl

/******************************************************************
**
**      FUNCTION NAME : main
**      FUNCTION TYPE : int
*/
int main( int argc, char * argv[] )
{
  INT << "===[START]==========================================" << endl;
  ///////////////////////////////////////////////////////////////////

  IPMutex mutex;
  {
    IPMutex::SharableLock slock( mutex );
    IPMutex::SharableLock slocka( IPMutex::SharableLock( mutex ) );
    LTAG( slock );
    sleep( 3 );
    {
      IPMutex::SharableLock slock2( mutex );
      LTAG( slock );
      LTAG( slock2 );
      sleep( 3 );
    }
    LTAG( slock );
    sleep( 3 );
  }

  ///////////////////////////////////////////////////////////////////
  INT << "===[END]============================================" << endl << endl;
  return 0;
}

