
#include <cstdio>

#include <chrono.hpp>
#include <fs.hpp>
#include <printer.hpp>
#include <sys.hpp>
#include <var.hpp>

#include <test/Test.hpp>

#include "usb.hpp"

class UnitTest : public test::Test {
public:

  UnitTest(var::StringView name) : test::Test(name) {}

	bool execute_class_api_case() {

		usb::Session session;



		return true;

	}

private:
};
