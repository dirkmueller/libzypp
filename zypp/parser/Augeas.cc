/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/

#include <iostream>
#include <stdlib.h>

#include "zypp/base/String.h"
#include "zypp/base/Exception.h"
#include "zypp/base/Gettext.h"
#include "zypp/base/Logger.h"
#include "zypp/Pathname.h"
#include "zypp/parser/Augeas.h"

using namespace zypp;
using namespace std;

namespace zypp {
namespace parser {

Augeas::Augeas(const string &module_lens, const string & filepath)
  : _augeas(NULL)
{
  //MIL << "Going to read zypper config using Augeas..." << endl;

  // init
  _augeas = ::aug_init(NULL, NULL, AUG_NO_LOAD | AUG_NO_MODL_AUTOLOAD);
  if (_augeas == NULL)
    ZYPP_THROW(Exception(_("Cannot initialize configuration file parser.")));

  if (::aug_set(_augeas, "/augeas/load/ZYpp/incl", filepath.c_str()) != 0)
      ZYPP_THROW(Exception(_("Augeas error: setting config file to load failed.")));

  if (::aug_set(_augeas, "/augeas/load/ZYpp/lens", module_lens.c_str()) != 0)
      ZYPP_THROW(Exception(_("Augeas error: setting the lens to load failed.")));

  // load the file
  if (aug_load(_augeas) != 0)
    ZYPP_THROW(Exception(_("Could not parse the config files.")));

  // collect eventual errors
  const char *value[1] = {};
  string error;

  ::aug_get(_augeas, (Pathname("/files") / filepath).c_str(), NULL) != 0;
  if (::aug_get(_augeas, (Pathname("/augeas/files") / filepath / "/error/message").c_str(), value))
      error = value[0];

  if (!error.empty()) {
      string msg = str::form(_("Error parsing %s\n %s"), filepath.c_str(), error.c_str());
    ZYPP_THROW(Exception(msg));
  }
}

// ---------------------------------------------------------------------------

Augeas::~Augeas()
{
  if (_augeas != NULL)
    ::aug_close(_augeas);
}

// ---------------------------------------------------------------------------

string Augeas::get(const string & augpath) const
{
  const char *value[1] = {};
  int result = ::aug_get(_augeas, augpath.c_str(), value);
  if (result != 0)
  {
    DBG << "Got " << augpath << " = " << value[0] << endl;
    return value[0];
  }
  else if (result == 0)
    DBG << "No match for " << augpath << endl;
  else
    DBG << "Multiple matches for " << augpath << endl;

  return string();
}

vector<string> Augeas::match(const string & augpath) const
{
  char **matches;

  int cnt = ::aug_match(_augeas, augpath.c_str(), &matches);
  if (cnt > 0)
  {
      DBG << "Got " << augpath << ": " << cnt << endl;
      vector<string> result(cnt);
      for (int i=0; i < cnt; i++) {
          result[i] = string(matches[i]);
      }
      return result;
  }
  else if (cnt == 0)
    DBG << "No match for " << augpath << endl;
  else
    DBG << "Multiple matches for " << augpath << endl;

  return vector<string>();
}


} // ns parser
} // ns zypp
