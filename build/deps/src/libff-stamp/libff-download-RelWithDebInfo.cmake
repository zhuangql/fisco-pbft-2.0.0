

set(command "/usr/bin/cmake;-Dmake=${make};-Dconfig=${config};-P;/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-download-RelWithDebInfo-impl.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-download-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-download-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "libff download command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/libff-stamp/libff-download-*.log")
  message(STATUS "${msg}")
endif()
