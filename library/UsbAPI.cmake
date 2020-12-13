
if(NOT DEFINED API_IS_SDK)
	include(JsonAPI)
	sos_sdk_include_target(UsbAPI "${API_CONFIG_LIST}")
endif()
