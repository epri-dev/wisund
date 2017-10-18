#include <iostream>
#include <iomanip>
#include <future>
#include <string>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include "Message.h"
#include "pcapng.h"
#include "Console.h"

class pcapngTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(pcapngTest);
    CPPUNIT_TEST(testSHB);
    CPPUNIT_TEST(testIDB);
    CPPUNIT_TEST(testEPB);
    CPPUNIT_TEST_SUITE_END();
public:
    void testSHB() {
        pcapng::SHB shb;  // default constructed SHB
        std::stringstream ss;
        ss << shb;
        const std::string desired{R"(
BlockType = 0xa0d0d0a
len = 0x1c
ByteOrderMagic = 0x1a2b3c4d
MajorVersion = 0x1
MinorVersion = 0x0
SectionLen = 0xffffffffffffffff
)"};
        std::cout << "\"" << ss.str() << "\"\n";
        std::cout << "\"" << desired << "\"\n";
        CPPUNIT_ASSERT(ss.str() == desired);
    }
    void testIDB() {
        pcapng::IDB shb;  // default constructed IDB
        std::stringstream ss;
        ss << shb;
        const std::string desired{R"(
BlockType = 0x1
len = 0x14
LinkType = 0xe6
Reserved = 0x0
SnapLen = 0xffff
)"};
        std::cout << "\"" << ss.str() << "\"\n";
        std::cout << "\"" << desired << "\"\n";
        CPPUNIT_ASSERT(ss.str() == desired);
    }
    void testEPB() {
        pcapng::EPB shb;  // default constructed EPB
        std::stringstream ss;
        // overwrite timestamp with known value
        shb.TimestampHi=0x55bc5;
        shb.TimestampLo=0x4b51f75e;
        ss << shb;
        const std::string desired{R"(
BlockType = 0x6
len = 0x20
InterfaceID = 0x0
TimestampHi = 0x55bc5
TimestampLo = 0x4b51f75e
CapturedLen = 0x0
OriginalLen = 0x0
)"};
        std::cout << "\"" << ss.str() << "\"\n";
        std::cout << "\"" << desired << "\"\n";
        CPPUNIT_ASSERT(ss.str() == desired);
    }

private:
};

CPPUNIT_TEST_SUITE_REGISTRATION(pcapngTest);

int main()
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run();
  std::cout << "wasSuccessful = " << std::boolalpha << wasSuccessful << '\n';
  return !wasSuccessful;
}
