# Test clang on UNIX using Visual Studio Team Services

set -ue
set -o pipefail
set -x

cd ${LLVM_OBJ_DIR}
make -j${BUILD_CPU_COUNT} check-checkedc

if [ "${TEST_SUITE}" == "CheckedC_clang" ]; then
  make -j${BUILD_CPU_COUNT} check-clang
elif [ "${TEST_SUITE}" == "CheckedC_LLVM" ]; then
  make -j${BUILD_CPU_COUNT} check-all
fi

set +ue
set +o pipefail


