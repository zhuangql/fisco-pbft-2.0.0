export "HUNTER_CELLAR_RAW_DIRECTORY=/home/xeniro/fisco-bco/FISCO-BCOS/deps/src/.hunter/_Base/Cellar/63c01d7795691cc6e5252ea8be5334ff5fa28638/63c01d7/raw"

ln -f \
  "${HUNTER_CELLAR_RAW_DIRECTORY}/include/intx/builtins.h" \
  "$1/include/intx/builtins.h"

ln -f \
  "${HUNTER_CELLAR_RAW_DIRECTORY}/include/intx/int128.hpp" \
  "$1/include/intx/int128.hpp"

ln -f \
  "${HUNTER_CELLAR_RAW_DIRECTORY}/include/intx/intx.hpp" \
  "$1/include/intx/intx.hpp"

ln -f \
  "${HUNTER_CELLAR_RAW_DIRECTORY}/lib/cmake/intx/intxConfig.cmake" \
  "$1/lib/cmake/intx/intxConfig.cmake"

ln -f \
  "${HUNTER_CELLAR_RAW_DIRECTORY}/lib/cmake/intx/intxConfigVersion.cmake" \
  "$1/lib/cmake/intx/intxConfigVersion.cmake"

ln -f \
  "${HUNTER_CELLAR_RAW_DIRECTORY}/lib/cmake/intx/intxTargets-release.cmake" \
  "$1/lib/cmake/intx/intxTargets-release.cmake"

ln -f \
  "${HUNTER_CELLAR_RAW_DIRECTORY}/lib/cmake/intx/intxTargets.cmake" \
  "$1/lib/cmake/intx/intxTargets.cmake"

ln -f \
  "${HUNTER_CELLAR_RAW_DIRECTORY}/lib/libintx.a" \
  "$1/lib/libintx.a"

ln -f \
  "${HUNTER_CELLAR_RAW_DIRECTORY}/licenses/intx/LICENSE" \
  "$1/licenses/intx/LICENSE"

