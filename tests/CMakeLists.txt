

set(DEPENDENCIES TestAPI FsAPI VarAPI)

stratify_api_add_test_executable(UsbAPI 32768 "${DEPENDENCIES}")



