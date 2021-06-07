
export LD_LIBRARY_PATH=/node-shared/lib:${LD_LIBRARY_PATH}

env PATH=/node-shared/vcpkg/installed/x64-linux/tools/protobuf:$PATH \
    CXXFLAGS="-I/node-shared/vcpkg/installed/x64-linux/include -DROCKSDB_PLATFORM_POSIX=1 -I/node-shared/leipeng/osc/topling-core/src" \
    LDFLAGS="-L/node-shared/lib" \
    PROTO_BUF_LDFLAGS="-L/node-shared/lib -letcd-cpp-api" \
    ROCKSDB_PATH="/node-shared/leipeng/osc/rocksdb" \
    make DISABLE_UPDATE_SB=1 DISABLE_WARNING_AS_ERROR=1 $@
