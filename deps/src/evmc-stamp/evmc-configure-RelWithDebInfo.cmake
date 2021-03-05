

set(command "/usr/bin/cmake;-DCMAKE_INSTALL_PREFIX=/home/xeniro/fisco-bco/FISCO-BCOS/deps;-GUnix Makefiles;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmc")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmc-stamp/evmc-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmc-stamp/evmc-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmc-stamp/evmc-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "evmc configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmc-stamp/evmc-configure-*.log")
  message(STATUS "${msg}")
endif()
