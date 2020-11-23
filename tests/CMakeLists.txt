

set(DEPENDENCIES UsbAPI TestAPI FsAPI VarAPI)

api_add_test_executable(UsbAPI 32768 "${DEPENDENCIES}")



