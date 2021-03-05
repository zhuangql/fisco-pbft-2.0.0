

set(command "/usr/bin/cmake;-DCMAKE_INSTALL_PREFIX=/home/xeniro/fisco-bco/FISCO-BCOS/build/deps;-DCMAKE_INSTALL_LIBDIR=lib;-DCMAKE_BUILD_TYPE=Release;-DMPIR_GMP=On;-GUnix Makefiles;/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/mpir")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/mpir-stamp/mpir-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/mpir-stamp/mpir-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/mpir-stamp/mpir-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "mpir configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/build/deps/src/mpir-stamp/mpir-configure-*.log")
  message(STATUS "${msg}")
endif()
