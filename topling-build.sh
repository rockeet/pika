
env PATH=/node-shared/vcpkg/installed/x64-linux/tools/protobuf:$PATH \
    CXXFLAGS="-I/node-shared/vcpkg/installed/x64-linux/include -DROCKSDB_PLATFORM_POSIX=1" \
    LDFLAGS="-L/node-shared/vcpkg/installed/x64-linux/lib" \
    make DISABLE_UPDATE_SB=1 V=1 -j70
