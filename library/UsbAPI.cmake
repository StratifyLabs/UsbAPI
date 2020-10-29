
if(NOT DEFINED IS_SDK)
	include(JsonAPI)
	sos_sdk_include_target(UsbAPI "${STRATIFYAPI_CONFIG_LIST}")
endif()
