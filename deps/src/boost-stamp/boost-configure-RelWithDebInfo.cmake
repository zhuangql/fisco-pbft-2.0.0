

set(command "./bootstrap.sh")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/boost-stamp/boost-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/boost-stamp/boost-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/boost-stamp/boost-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "boost configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/boost-stamp/boost-configure-*.log")
  message(STATUS "${msg}")
endif()
