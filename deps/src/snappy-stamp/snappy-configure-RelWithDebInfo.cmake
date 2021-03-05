

set(command "/usr/bin/cmake;-DCMAKE_INSTALL_PREFIX=/home/xeniro/fisco-bco/FISCO-BCOS/deps;-DCMAKE_POSITION_INDEPENDENT_CODE=OFF;-DCMAKE_BUILD_TYPE=Release;-DSNAPPY_BUILD_TESTS=OFF;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-GUnix Makefiles;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/snappy")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/snappy-stamp/snappy-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/snappy-stamp/snappy-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/snappy-stamp/snappy-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "snappy configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/snappy-stamp/snappy-configure-*.log")
  message(STATUS "${msg}")
endif()
