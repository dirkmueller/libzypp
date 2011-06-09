#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <zypp/parser/Augeas.h>
#include <zypp/base/NonCopyable.h>

#include "TestSetup.h"

using namespace std;
using namespace zypp;

// Must be the first test!
BOOST_AUTO_TEST_CASE(augeas_read_repo_file)
{
  parser::Augeas augeas("Yum.lns", TESTS_SRC_DIR "/parser/inifile/data/*.ini");
  {
      vector<string> ret = augeas.match(("/files" TESTS_SRC_DIR "/parser/inifile/data/*/*"));

      // 8 repositories in the fixtures
      BOOST_CHECK_EQUAL(8, ret.size());

      // check that some elements are there
      BOOST_CHECK(find(ret.begin(), ret.end(),
                       "/files" TESTS_SRC_DIR "/parser/inifile/data/1.ini/base") != ret.end());
      BOOST_CHECK(find(ret.begin(), ret.end(),
                       "/files" TESTS_SRC_DIR "/parser/inifile/data/2.ini/equal") != ret.end());

      BOOST_CHECK_EQUAL("http://mirror.centos.org/centos/RPM-GPG-KEY-centos4",
                        augeas.get("/files" TESTS_SRC_DIR "/parser/inifile/data/1.ini/update/gpgkey"));

      BOOST_CHECK_EQUAL(string(),
                        augeas.get("/files/wrongpath"));

      // all repos
      for_ (it, ret.begin(), ret.end()) {
          cout << *it << endl;;
      }
  }
}
