

set(command "/usr/bin/cmake;-DCMAKE_INSTALL_PREFIX=/home/xeniro/fisco-bco/FISCO-BCOS/deps;-DBUILD_SHARED_LIBS=off;-DEVMC_ROOT=/home/xeniro/fisco-bco/FISCO-BCOS/deps;-DHUNTER_ROOT=/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/.hunter;-GUnix Makefiles;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmone")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmone-stamp/evmone-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmone-stamp/evmone-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmone-stamp/evmone-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "evmone configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmone-stamp/evmone-configure-*.log")
  message(STATUS "${msg}")
endif()
