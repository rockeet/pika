
if test "${CXX}A" = "A"; then
	echo use default CXX=g++
	CXX=g++
fi

set -e

ROCKSDB_DIR=${ROCKSDB_DIR:-"../rocksdb"}
TOPLING_CORE_DIR=${TOPLING_CORE_DIR:-"../topling-core"}
WITH_BMI2=`bash ${TOPLING_CORE_DIR}/cpu_has_bmi2.sh`
tmpfile=`mktemp -u compiler-XXXXXX`
COMPILER=`${CXX} ${TOPLING_CORE_DIR}/tools/configure/compiler.cpp -o ${tmpfile}.exe && ./${tmpfile}.exe`
rm -f ${tmpfile}*
UNAME_MachineSystem=`uname -m -s | sed 's:[ /]:-:g'`
BUILD_NAME=${UNAME_MachineSystem}-${COMPILER}-bmi2-${WITH_BMI2}
#BUILD_ROOT=build/${BUILD_NAME}

LIB_SUFFIX_ARRAY=(r a d) # rls afr dbg
LIB_SUFFIX=`eval 'echo ${LIB_SUFFIX_ARRAY['$DEBUG_LEVEL']}'`
export LD_LIBRARY_PATH=/node-shared/lib:${LD_LIBRARY_PATH}

env PATH=/node-shared/vcpkg/installed/x64-linux/tools/protobuf:$PATH \
    CXXFLAGS="-DJSON_USE_GOLD_HASH_MAP=1 \
              -DROCKSDB_PLATFORM_POSIX=1 \
              -I/node-shared/vcpkg/installed/x64-linux/include \
              -I/node-shared/leipeng/osc/rocksdb/sideplugin/rockside/src \
              -I/node-shared/leipeng/osc/rocksdb/sideplugin/topling-core/boost-include \
              -I/node-shared/leipeng/osc/rocksdb/sideplugin/topling-rocks/src \
              -I/node-shared/leipeng/osc/topling-core/src" \
    LDFLAGS="-L/node-shared/lib -lterark-core-${COMPILER}-${LIB_SUFFIX}" \
    PROTO_BUF_LDFLAGS="-L/node-shared/lib -letcd-cpp-api" \
    ROCKSDB_PATH="/node-shared/leipeng/osc/rocksdb" \
    make DISABLE_UPDATE_SB=1 DISABLE_WARNING_AS_ERROR=1 $@

shopt -s nullglob
for fpath in third/blackwidow/lib/libblackwidow*.so; do
  fname=`basename $fpath`
  rm -f  /node-shared/lib/$fname
  cp -ap $fpath /node-shared/lib
done
