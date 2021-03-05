

set(command "/usr/bin/cmake;-DCMAKE_INSTALL_PREFIX=/home/xeniro/fisco-bco/FISCO-BCOS/deps;-DCMAKE_BUILD_TYPE=Release;-DCMAKE_POSITION_INDEPENDENT_CODE=OFF;-DBUILD_STATIC_LIBS=ON;-DBUILD_SHARED_LIBS=OFF;-DUNIX_DOMAIN_SOCKET_SERVER=OFF;-DUNIX_DOMAIN_SOCKET_CLIENT=OFF;-DHTTP_SERVER=ON;-DHTTP_CLIENT=OFF;-DCOMPILE_TESTS=OFF;-DCOMPILE_STUBGEN=OFF;-DCOMPILE_EXAMPLES=OFF;-DJSONCPP_INCLUDE_DIR=/home/xeniro/fisco-bco/FISCO-BCOS/deps/include;-DJSONCPP_INCLUDE_PREFIX=json;-DJSONCPP_LIBRARY=/home/xeniro/fisco-bco/FISCO-BCOS/deps/lib/libjsoncpp.a;-DMHD_INCLUDE_DIR=/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/mhd/src/include/;-DMHD_LIBRARY=/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/mhd/src/microhttpd/.libs/libmicrohttpd.a;-DCMAKE_C_FLAGS=-fvisibility=hidden;-fvisibility-inlines-hidden;-DCMAKE_CXX_FLAGS=-fvisibility=hidden;-fvisibility-inlines-hidden;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-GUnix Makefiles;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp-stamp/jsonrpccpp-configure-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp-stamp/jsonrpccpp-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp-stamp/jsonrpccpp-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "jsonrpccpp configure command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/jsonrpccpp-stamp/jsonrpccpp-configure-*.log")
  message(STATUS "${msg}")
endif()
