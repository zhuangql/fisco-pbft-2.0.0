

set(command "bash;-c;/bin/cp /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/libvrf/libffi_vrf_generic64.a /home/xeniro/fisco-bco/FISCO-BCOS/deps/lib/libffi_vrf.a")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/libvrf-stamp/libvrf-install-out.log"
  ERROR_FILE "/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/libvrf-stamp/libvrf-install-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/libvrf-stamp/libvrf-install-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "libvrf install command succeeded.  See also /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/libvrf-stamp/libvrf-install-*.log")
  message(STATUS "${msg}")
endif()
