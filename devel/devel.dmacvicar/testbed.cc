#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>

#include "boost/filesystem/operations.hpp" // includes boost/filesystem/path.hpp
#include "boost/filesystem/fstream.hpp"    // ditto

#include <boost/iostreams/device/file_descriptor.hpp>

#include <zypp/base/Logger.h>
#include <zypp/Locale.h>
#include <zypp/ZYpp.h>
#include <zypp/ZYppFactory.h>
#include <zypp/TranslatedText.h>
///////////////////////////////////////////////////////////////////

#include <zypp/base/Logger.h>


#include <map>
#include <set>

#include "zypp/CapFactory.h"
#include "zypp/KeyRing.h"
#include "zypp/PublicKey.h"

#include "zypp/MediaSetAccess.h"

using namespace zypp::detail;

using namespace std;
using namespace zypp;
using namespace zypp::source;
//using namespace DbXml;

int main()
{
  MediaSetAccess ma( Url("cd:///"), Pathname("/"));
  MIL << "done 1" << srd::endl;
  Pathname local = ma.provideFile("content", 1);
  MIL << local << std::endl;
  
}


