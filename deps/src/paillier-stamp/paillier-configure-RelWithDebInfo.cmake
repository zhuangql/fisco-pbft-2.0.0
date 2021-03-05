

set(command "/usr/bin/cmake;-DCMAKE_INSTALL_PREFIX=/home/xeniro/fisco-bco/FISCO-BCOS/deps;-DCMAKE_POSITION_INDEPENDENT_CODE=ON;-GUnix Makefiles;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/paillier")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/paillier-stamp/paillier-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/paillier-stamp/paillier-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/paillier-stamp/paillier-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "paillier configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/paillier-stamp/paillier-configure-*.log")
  message(STATUS "${msg}")
endif()
