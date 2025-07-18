CURRENT_DIR=$(pwd)
[-d build_dir] && rm -r build_dir
mkdir build_dir
cd build_dir
cmake ..
make

./test_boost_fsm
