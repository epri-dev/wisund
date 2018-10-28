#include <iostream>
#include <iomanip>
#include <future>
#include <string>
#include <sstream>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include "Message.h"

class MessageTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MessageTest);
    CPPUNIT_TEST(testBasicMessage);
    CPPUNIT_TEST(capMessage);
    CPPUNIT_TEST_SUITE_END();
public:
    void testBasicMessage() {
        CPPUNIT_ASSERT(isRaw(m1) == false);
        CPPUNIT_ASSERT(m1.size() == 3);
        std::cout << "m1 = " << m1 << '\n';
        CPPUNIT_ASSERT(isRaw(m2) == true);
        CPPUNIT_ASSERT(m2.size() == 3);
        std::cout << "m2 = " << m2 << '\n';
    }
    void capMessage() {
        Message capmsg{0x31, 0x82, 0x82};
        CPPUNIT_ASSERT(isCap(capmsg));
        CPPUNIT_ASSERT(capmsg.size() == 3);
    }
    void setUp() {
    }
    void tearDown() {
    }
private:
	Message m1{0x05, 0x7f, 0x32};
	Message m2{0x00, 0x7f, 0x32};
};


CPPUNIT_TEST_SUITE_REGISTRATION(MessageTest);

int main()
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run();
  std::cout << "wasSuccessful = " << std::boolalpha << wasSuccessful << '\n';
  return !wasSuccessful;
}
