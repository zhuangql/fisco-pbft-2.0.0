

set(command "/usr/bin/cmake;--build;/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-build;--config;Release")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-build-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-build-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-build-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "libff build command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-build-*.log")
  message(STATUS "${msg}")
endif()
