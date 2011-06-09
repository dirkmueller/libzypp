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

#include "zypp/base/Easy.h"
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

Augeas::Augeas(const string &module_lens, const string & fileExpr)
  : _augeas(NULL)
{
  //MIL << "Going to read zypper config using Augeas..." << endl;

  // init
  _augeas = ::aug_init(NULL, NULL, AUG_NO_LOAD | AUG_NO_MODL_AUTOLOAD);
  if (_augeas == NULL)
    ZYPP_THROW(Exception(_("Cannot initialize configuration file parser.")));


  if (::aug_set(_augeas, "/augeas/load/ZYpp/lens", module_lens.c_str()) != 0)
      ZYPP_THROW(Exception(_("Augeas error: setting the lens to load failed.")));

  includeExpression(fileExpr);
  load();
  throwErrors();
}

// ---------------------------------------------------------------------------

Augeas::~Augeas()
{
  if (_augeas != NULL)
    ::aug_close(_augeas);
}

// ---------------------------------------------------------------------------

void Augeas::throwErrors()
{
  Exception augExcpt(_("Problem when parsing configuration files"));

  vector<string> errors(match("/augeas//error"));
  for_(it, errors.begin(), errors.end()) {
    augExcpt.addHistory(get(*it));
  }

  if (!augExcpt.historyEmpty())
      ZYPP_THROW(augExcpt);
}

// ---------------------------------------------------------------------------

void Augeas::includeExpression(const std::string &fileExpr)
{
  if (::aug_set(_augeas, "/augeas/load/ZYpp/incl", fileExpr.c_str()) != 0)
      ZYPP_THROW(Exception(_("Augeas error: setting config file to load failed.")));
}

// ---------------------------------------------------------------------------

void Augeas::load()
{
  // load the file
  if (aug_load(_augeas) != 0)
    ZYPP_THROW(Exception(_("Could not parse the config files.")));
}

// ---------------------------------------------------------------------------

string Augeas::get(const string & augpath) const
{
  const char *value[1] = {};
  int result = ::aug_get(_augeas, augpath.c_str(), value);
  if (result != 0)
  {
    DBG << "Got " << augpath << " = " << value[0] << endl;
    return value[0] ? value[0] : string();
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
