make -C build >> /dev/null
make -C build unit-tests | grep -v make
