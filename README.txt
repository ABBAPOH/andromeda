To build Andromeda, make sure that Qt is installed on your system.
You can download Qt from http://qt.nokia.com/downloads
On Linux, you can also install it using package manager.

Also you need to install or build QBS. Source code can be found at http://gitorious.org/qt-labs.
Instructions for building and using QBS are here http://doc-snapshot.qt-project.org/qbs/

To clone Andromeda repository, open terminal and run following commands from
the directory you want to use:
$ git clone git@gitorious.org:andromeda/andromeda.git
(OR '$ git clone git@github.com:ABBAPOH/andromeda.git')
$ cd andromeda
$ git submodule init
$ git submodule update

After cloning, you can build Andromeda:
$ cd .. (one dir up from sources)
$ mkdir andromeda-build
$ cd andromeda-build
$ qbs install -f ../andromeda/andromeda.qbs release

QBS will create directory with the name of you default QBS profile and install
Andromeda to the 'install_root' folder within that directory.
