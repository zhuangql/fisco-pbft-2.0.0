

set(command "/usr/bin/cmake;-DCMAKE_BUILD_TYPE=Release;-DCMAKE_INSTALL_PREFIX=/home/xeniro/fisco-bco/FISCO-BCOS/build/deps;-DGMP_INCLUDE_DIR=/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/include;-DGMP_LIBRARY=/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/lib/libmpir.a;-DCURVE=ALT_BN128;-DPERFORMANCE=OFF;-DWITH_PROCPS=OFF;-DUSE_PT_COMPRESSION=OFF;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-DCMAKE_CXX_FLAGS=-std=c++11 -pthread -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -fexceptions;-GUnix Makefiles;/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "libff configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-configure-*.log")
  message(STATUS "${msg}")
endif()
