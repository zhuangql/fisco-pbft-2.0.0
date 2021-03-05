

set(command "./configure;--disable-shared;CXXFLAGS=-DHAVE_POSIX_MEMALIGN_SYMBOL=1;--enable-frame-pointers;--disable-cpu-profiler;--disable-heap-profiler;--disable-heap-checker;--disable-debugalloc;--enable-minimal;--prefix=/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/gperftools")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps//src/gperftools-stamp/gperftools-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps//src/gperftools-stamp/gperftools-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps//src/gperftools-stamp/gperftools-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "gperftools configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps//src/gperftools-stamp/gperftools-configure-*.log")
  message(STATUS "${msg}")
endif()
