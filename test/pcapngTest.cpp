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
#include "pcapng.h"
#include "Console.h"

bool operator==(const Message &a, const Message &b) {
    std::cout << "calling == with " << a << " and " << b << "\n";
    if (a.size() != b.size())
        return false;
    auto bitem = b.begin();
    for (const auto &aitem : a) {
        if (aitem != *bitem)
            return false;
        ++bitem;
    }
    return true;
}

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
    struct MessagePair {
        const Message enc, dec;
    };
    static std::vector<MessagePair> tstmsg;
};

/*
received: c03101e3a1bc01ff0ffeff59190005150100d782f0003f1fa00688fa050a1001010504e8030000230e05455052492d576953554e2d46414eb4a10a10c0
received: c03109e3a1bc01ff0ffeff5919000e0531000001051501022b0000071502070000000000003f3ca00688fa050a1001010c9000a60e002a0078050a10010102060100200721436587000000002243658700000000234365870000000024436587000000002ea22a3323e78ff434610000c0 
*/

std::vector<pcapngTest::MessagePair> pcapngTest::tstmsg{
    { Message{0xc0, 0xc0}, Message{} },
    { Message{0xc0, 0x00, 0xdb, 0xdc, 0x00, 0xc0}, Message{0x00, 0xc0, 0x00} },
    { Message{0xc0, 0x00, 0xdb, 0xdd, 0x00, 0xc0}, Message{0x00, 0xdb, 0x00} },
    { Message{0xc0, 0x00, 0xdd, 0x00, 0xc0}, Message{0x00, 0xdd, 0x00} },
    { Message{0xc0, 0x00, 0xdc, 0x00, 0xc0}, Message{0x00, 0xdc, 0x00} },
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
