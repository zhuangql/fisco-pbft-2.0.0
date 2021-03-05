

set(command "/usr/bin/cmake;--build;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp-build;--config;Release;--target;install")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp-stamp/jsonrpccpp-install-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp-stamp/jsonrpccpp-install-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp-stamp/jsonrpccpp-install-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "jsonrpccpp install command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp-stamp/jsonrpccpp-install-*.log")
  message(STATUS "${msg}")
endif()
