# Install script for directory: /home/xeniro/fisco-bco/FISCO-BCOS

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libchannelserver/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libdevcore/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libdevcrypto/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libethcore/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libstat/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libflowlimit/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libtxpool/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libstorage/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libprecompiled/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libnetwork/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libp2p/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libexecutive/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libmptstate/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libblockverifier/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libstoragestate/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libblockchain/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libsync/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libconsensus/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libledger/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/librpc/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libinitializer/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libsecurity/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter/cmake_install.cmake")
  include("/home/xeniro/fisco-bco/FISCO-BCOS/build/fisco-bcos/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/xeniro/fisco-bco/FISCO-BCOS/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
