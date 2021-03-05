

set(command "bash;-c;/bin/cp -f ./build/*_release/libtbb.a* /home/xeniro/fisco-bco/FISCO-BCOS/deps/lib/")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tbb-stamp/tbb-install-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tbb-stamp/tbb-install-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tbb-stamp/tbb-install-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "tbb install command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tbb-stamp/tbb-install-*.log")
  message(STATUS "${msg}")
endif()
