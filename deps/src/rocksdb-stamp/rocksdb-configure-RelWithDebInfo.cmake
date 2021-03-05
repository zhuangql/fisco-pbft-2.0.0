

set(command "/usr/bin/cmake;-DCMAKE_INSTALL_PREFIX=/home/xeniro/fisco-bco/FISCO-BCOS/deps;-DCMAKE_POSITION_INDEPENDENT_CODE=ON;-DCMAKE_BUILD_TYPE=Release;-DWITH_LZ4=OFF;-DWITH_SNAPPY=ON;-DWITH_GFLAGS=OFF;-DWITH_TESTS=OFF;-DWITH_TOOLS=OFF;-DBUILD_SHARED_LIBS=OFF;-DUSE_RTTI=ON;-DCMAKE_C_FLAGS=-std=c99;-march=x86-64 -mtune=generic -fvisibility=hidden -fvisibility-inlines-hidden;-DCMAKE_CXX_FLAGS=-std=c++11 -pthread -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -fexceptions;-march=x86-64 -mtune=generic -fvisibility=hidden -fvisibility-inlines-hidden;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-GUnix Makefiles;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "rocksdb configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-configure-*.log")
  message(STATUS "${msg}")
endif()
