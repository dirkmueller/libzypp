#include <sstream>
#include <string>
#include <zypp/parser/Augeas.h>
#include <zypp/base/NonCopyable.h>

#include "TestSetup.h"

using std::stringstream;
using std::string;
using namespace zypp;

static string suse_repo = "[factory-oss]\n"
"name=factory-oss\n"
"enabled=1\n"
"autorefresh=0\n"
"baseurl=http://download.opensuse.org/factory-tested/repo/oss/\n"
"type=yast2\n"
"keeppackages=0\n";

static string fedora_repo = "[fedora]\n"
"name=Fedora $releasever - $basearch\n"
"failovermethod=priority\n"
"#baseurl=http://download.fedora.redhat.com/pub/fedora/linux/releases/$releasever/Everything/$basearch/os/\n"
"#mirrorlist=http://mirrors.fedoraproject.org/mirrorlist?repo=fedora-$releasever&arch=$basearch\n"
"mirrorlist=file:///etc/yum.repos.d/local.mirror\n"
"enabled=1\n"
"gpgcheck=1\n"
"gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-fedora file:///etc/pki/rpm-gpg/RPM-GPG-KEY\n";

// Must be the first test!
BOOST_AUTO_TEST_CASE(read_repo_file)
{
  parser::Augeas augeas("Yum.lns", "/etc/zypp/repos.d/systemsmanagement_wbem.repo");
  {
      BOOST_CHECK_EQUAL("lala", augeas.get("/files/etc/zypp/repos.d/systemsmanagement_wbem.repo"));

     BOOST_CHECK_EQUAL(0, augeas.match("/files/etc/zypp/repos.d/systemsmanagement_wbem.repo/*").size());

  }
}
