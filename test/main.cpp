#include <iostream>
#include <iomanip>
#include <string>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include "Message.h"

class MessageTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MessageTest);
    CPPUNIT_TEST(testBasic);
    CPPUNIT_TEST_SUITE_END();
public:
    void testBasic() {
        CPPUNIT_ASSERT(m1.isRaw() == false);
        CPPUNIT_ASSERT(m1.size() == 3);
        std::cout << "m1 = " << m1 << '\n';
    }
    void setUp() {
    }
    void tearDown() {
    }
private:
	Message m1{0x05, 0x7f, 0x32};
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
