

set(command "make;extra_inc=big_iron.inc;cpp0x=1")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tbb-stamp/tbb-build-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tbb-stamp/tbb-build-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tbb-stamp/tbb-build-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "tbb build command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/tbb-stamp/tbb-build-*.log")
  message(STATUS "${msg}")
endif()
