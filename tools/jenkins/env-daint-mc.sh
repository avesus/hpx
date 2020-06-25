source /apps/daint/SSL/HPX/spack/share/spack/setup-env.sh

export CRAYPE_LINK_TYPE=dynamic

export LOCAL_ROOT="/apps/daint/UES/simbergm/local"
export GCC_VER="10.1.0"
export CXX_STD="17"
export BOOST_VER="1.73.0"
export TCMALLOC_VER="2.7"
export HWLOC_VER="2.2.0"
export GCC_ROOT="${LOCAL_ROOT}/gcc-${GCC_VER}"
export BOOST_ROOT="${LOCAL_ROOT}/boost-${BOOST_VER}-gcc-${GCC_VER}-c++${CXX_STD}-debug"
export HWLOC_ROOT="${LOCAL_ROOT}/hwloc-${HWLOC_VER}-gcc-${GCC_VER}"
export CXXFLAGS="-nostdinc++ -I${GCC_ROOT}/include/c++/${GCC_VER} -I${GCC_ROOT}/include/c++/${GCC_VER}/x86_64-unknown-linux-gnu -I${GCC_ROOT}/include/c++/${GCC_VER}/x86_64-pc-linux-gnu -L${GCC_ROOT}/lib64 -Wl,-rpath,${GCC_ROOT}/lib64"
export LDFLAGS="-L${GCC_ROOT}/lib64"
export CXX=${GCC_ROOT}/bin/g++
export CC=${GCC_ROOT}/bin/gcc

module load daint-mc
spack load cmake
spack load ninja
