

set(command "./b2;stage;cxxflags=-march=x86-64 -mtune=generic -fvisibility=hidden -fvisibility-inlines-hidden;threading=multi;link=static;variant=release;address-model=64;--with-chrono;--with-date_time;--with-filesystem;--with-system;--with-random;--with-regex;--with-test;--with-thread;--with-serialization;--with-program_options;--with-log;-j2")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/boost-stamp/boost-build-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/boost-stamp/boost-build-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/boost-stamp/boost-build-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "boost build command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/boost-stamp/boost-build-*.log")
  message(STATUS "${msg}")
endif()
