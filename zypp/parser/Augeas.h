/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/

#ifndef ZYPP_PARSER_AUGEAS_H_
#define ZYPP_PARSER_AUGEAS_H_

#include <string>
#include <vector>

extern "C"
{
  #include <augeas.h>
}

#include "zypp/base/NonCopyable.h"
#include "zypp/TriBool.h"

namespace zypp {
namespace parser {

/**
 * Wrapper around augeas
 *
 * Allows to manipulate configuration files based on augeas
 * expressions
 */
class Augeas : private zypp::base::NonCopyable
{
public:

  /**
   * Creates a handle to manipulate a specific file (or files)
   * with an specific lens
   *
   * Example:
   *
   * \code
   *   Augeas augeas("Yum.lns", "/etc/zypp/repos.d/foo.repo");
   * \endcode
   */
  Augeas(const std::string &module_lens, const std::string & file = "");
  ~Augeas();

  /**
   * Add files
   * Example:
   * \code
   *   augeas.includeExpression("/etc/zypp/repos.d/SLE-*.repo");
   * \endcode
   */
  void includeExpression(const std::string &fileExpr);

  /**
   * Loads the virtual /files tree
   */
  void load();

  std::string get(const std::string & augpath) const;

  std::vector<std::string> match(const std::string & augpath) const;

private:

  /**
   * Throws an exception for pending augeas errors
   */
  void throwErrors();

  ::augeas * _augeas;
};

}
}

#endif /* ZYPP_PARSER_AUGEAS_H_ */
