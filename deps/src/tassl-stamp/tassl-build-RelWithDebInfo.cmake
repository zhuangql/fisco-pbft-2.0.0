

set(command "make")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tassl-stamp/tassl-build-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tassl-stamp/tassl-build-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tassl-stamp/tassl-build-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "tassl build command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tassl-stamp/tassl-build-*.log")
  message(STATUS "${msg}")
endif()
