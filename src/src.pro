# Under Windows launch script start_qgit.bat needs the
# value GIT_EXEC_DIR to be set to the git bin directory
GIT_EXEC_DIR = "$(ProgramFiles)\\Git\\bin"

# Under Windows uncomment following line to enable console messages
#CONFIG += ENABLE_CONSOLE_MSG

# check for Qt >= 4.3.0
CUR_QT = $$[QT_VERSION]

# WARNING greaterThan is an undocumented function
!greaterThan(CUR_QT, 4.3) {
    error("Sorry I need Qt 4.3.0 or later, you seem to have Qt $$CUR_QT instead")
}

# check for g++ compiler
contains(QMAKE_CC,.*g\\+\\+.*) {
    CONFIG += HAVE_GCC
}
contains(QMAKE_CC,.*gcc.*) {
    CONFIG += HAVE_GCC
}

# General stuff
TEMPLATE = app
CONFIG += qt warn_on exceptions debug_and_release
INCLUDEPATH += ../src
MAKEFILE = qmake
RESOURCES += icons.qrc

# Platform dependent stuff
win32 {
    TARGET = qgit
    target.path = $$GIT_EXEC_DIR
    CONFIG += windows embed_manifest_exe
    RC_FILE = app_icon.rc
}

unix {
    TARGET = qgit
    target.path = ~/bin
    CONFIG += x11
}

macx {
    TARGET = qgit
    target.path = ~/bin
    #CONFIG += x86 ppc
    RC_FILE = resources/qgit.icns
}

HAVE_GCC {
    QMAKE_CXXFLAGS_RELEASE += -s -O2 -Wno-non-virtual-dtor -Wno-long-long -pedantic -Wconversion
    QMAKE_CXXFLAGS_DEBUG += -g3 -ggdb -O0 -Wno-non-virtual-dtor -Wno-long-long -pedantic -Wconversion
}

ENABLE_CONSOLE_MSG {
    CONFIG -= windows
    CONFIG += console
}

INSTALLS += target

# Directories
DESTDIR = ../bin
BUILD_DIR = ../build
UI_DIR = $$BUILD_DIR
MOC_DIR = $$BUILD_DIR
RCC_DIR = $$BUILD_DIR
OBJECTS_DIR = $$BUILD_DIR

# project files
FORMS += commit.ui console.ui customaction.ui fileview.ui help.ui \
         mainview.ui patchview.ui revsview.ui settings.ui \
    ui/rangeselect.ui

HEADERS += annotate.h cache.h commitimpl.h common.h config.h consoleimpl.h \
           customactionimpl.h dataloader.h domain.h exceptionmanager.h \
           filecontent.h filelist.h fileview.h git.h help.h lanes.h \
           listview.h mainimpl.h myprocess.h patchcontent.h patchview.h \
            revdesc.h revsview.h settingsimpl.h \
           treeview.h \
    branchestree.h \
    findsupport.h \
    branchestreeitem.h \
    externaldiffproc.h \
    reachinfo.h \
    rangeinfo.h \
    updatedomainevent.h \
    stateinfo.h \
    fileitem.h \
    diritem.h \
    linenumberarea.h \
    patchcontentfindsupport.h \
    patchtextblockuserdata.h \
    filehistory.h \
    listviewproxy.h \
    listviewdelegate.h \
    ui/rangeselectimpl.h \
    ui/customtabwidget.h \
    ui/customtab.h \
    model/shastring.h \
    model/revision.h \
    model/reference.h \
    model/referencelist.h \
    model/tagreference.h \
    model/stgitpatchreference.h \
    git/references.h \
    git/rungit_interface.h


SOURCES += annotate.cpp cache.cpp commitimpl.cpp consoleimpl.cpp \
           customactionimpl.cpp dataloader.cpp domain.cpp exceptionmanager.cpp \
           filecontent.cpp filelist.cpp fileview.cpp git.cpp \
           lanes.cpp listview.cpp mainimpl.cpp myprocess.cpp namespace_def.cpp \
           patchcontent.cpp patchview.cpp  \
           revdesc.cpp revsview.cpp settingsimpl.cpp treeview.cpp \
    branchestree.cpp \
    main.cpp \
    findsupport.cpp \
    branchestreeitem.cpp \
    externaldiffproc.cpp \
    reachinfo.cpp \
    rangeinfo.cpp \
    updatedomainevent.cpp \
    stateinfo.cpp \
    fileitem.cpp \
    diritem.cpp \
    linenumberarea.cpp \
    patchcontentfindsupport.cpp \
    patchtextblockuserdata.cpp \
    filehistory.cpp \
    listviewproxy.cpp \
    listviewdelegate.cpp \
    ui/rangeselectimpl.cpp \
    ui/customtabwidget.cpp \
    ui/customtab.cpp \
    model/shastring.cpp \
    model/revision.cpp \
    model/reference.cpp \
    model/referencelist.cpp \
    model/tagreference.cpp \
    model/stgitpatchreference.cpp \
    git/references.cpp

DISTFILES += app_icon.rc helpgen.sh resources/* Src.vcproj todo.txt
DISTFILES += ../COPYING ../exception_manager.txt ../README ../README_WIN.txt
DISTFILES += ../qgit_inno_setup.iss ../QGit4.sln

# Here we generate a batch called start_qgit.bat used, under Windows only,
# to start qgit with proper PATH set.
#
# NOTE: qgit must be installed in git directory, among git exe files
# for this to work. If you install with 'make install' this is already
# done for you.
#
# Remember to set proper GIT_EXEC_DIR value at the beginning of this file
#
win32 {
    !exists($${GIT_EXEC_DIR}/git.exe) {
        error("I cannot found git files, please set GIT_EXEC_DIR in 'src.pro' file")
    }
    QGIT_BAT = ../start_qgit.bat
    CUR_PATH = $$system(echo %PATH%)
    LINE_1 = $$quote(set PATH=$$CUR_PATH;$$GIT_EXEC_DIR;)
    LINE_2 = $$quote(set PATH=$$CUR_PATH;)

    qgit_launcher.commands =    @echo @echo OFF > $$QGIT_BAT
    qgit_launcher.commands += && @echo $$LINE_1 >> $$QGIT_BAT
    qgit_launcher.commands += && @echo bin\\$$TARGET >> $$QGIT_BAT
    qgit_launcher.commands += && @echo $$LINE_2 >> $$QGIT_BAT

    QMAKE_EXTRA_TARGETS += qgit_launcher
    PRE_TARGETDEPS += qgit_launcher
}
