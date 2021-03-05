set(command "/usr/bin/cmake;-P;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmone-stamp/download-evmone.cmake")

execute_process(COMMAND ${command} RESULT_VARIABLE result)
if(result)
  set(msg "Command failed (${result}):\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  message(FATAL_ERROR "${msg}")
endif()
set(command "/usr/bin/cmake;-P;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmone-stamp/verify-evmone.cmake")

execute_process(COMMAND ${command} RESULT_VARIABLE result)
if(result)
  set(msg "Command failed (${result}):\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  message(FATAL_ERROR "${msg}")
endif()
set(command "/usr/bin/cmake;-P;/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/evmone-stamp/extract-evmone.cmake")

execute_process(COMMAND ${command} RESULT_VARIABLE result)
if(result)
  set(msg "Command failed (${result}):\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  message(FATAL_ERROR "${msg}")
endif()
