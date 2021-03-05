

set(command "bash;config;-DOPENSSL_PIC;no-shared")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tassl-stamp/tassl-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tassl-stamp/tassl-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tassl-stamp/tassl-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "tassl configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tassl-stamp/tassl-configure-*.log")
  message(STATUS "${msg}")
endif()
