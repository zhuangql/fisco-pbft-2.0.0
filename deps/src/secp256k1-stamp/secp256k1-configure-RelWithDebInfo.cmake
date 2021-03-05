

set(command "/usr/bin/cmake;-DCMAKE_INSTALL_PREFIX=/home/xeniro/fisco-bco/FISCO-BCOS/deps;-DCMAKE_POSITION_INDEPENDENT_CODE=OFF;-DCMAKE_BUILD_TYPE=Release;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-GUnix Makefiles;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/secp256k1")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/secp256k1-stamp/secp256k1-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/secp256k1-stamp/secp256k1-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/secp256k1-stamp/secp256k1-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "secp256k1 configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/secp256k1-stamp/secp256k1-configure-*.log")
  message(STATUS "${msg}")
endif()
