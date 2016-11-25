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
#include "SerialDevice.h"
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

class SerialTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SerialTest);
    CPPUNIT_TEST(testEncode);
    CPPUNIT_TEST(testDecode);
    CPPUNIT_TEST_SUITE_END();
public:
    void testDecode() {
        for (const auto &pair : tstmsg) {
            std::cout << pair.enc << " --> " << SerialDevice::decode(pair.enc) << "\n";
            CPPUNIT_ASSERT(SerialDevice::decode(pair.enc) == pair.dec);
        }
    }
    void testEncode() {
        for (const auto &pair : tstmsg) {
            std::cout << pair.dec << " --> " << SerialDevice::encode(pair.dec) << "\n";
            CPPUNIT_ASSERT(SerialDevice::encode(pair.dec) == pair.enc);
        }
    }

private:
    struct MessagePair {
        const Message enc, dec;
    };
    static std::vector<MessagePair> tstmsg;
};

std::vector<SerialTest::MessagePair> SerialTest::tstmsg{
    { Message{0xc0, 0xc0}, Message{} },
    { Message{0xc0, 0x00, 0xdb, 0xdc, 0x00, 0xc0}, Message{0x00, 0xc0, 0x00} },
    { Message{0xc0, 0x00, 0xdb, 0xdd, 0x00, 0xc0}, Message{0x00, 0xdb, 0x00} },
    { Message{0xc0, 0x00, 0xdd, 0x00, 0xc0}, Message{0x00, 0xdd, 0x00} },
    { Message{0xc0, 0x00, 0xdc, 0x00, 0xc0}, Message{0x00, 0xdc, 0x00} },
};

CPPUNIT_TEST_SUITE_REGISTRATION(SerialTest);

int main()
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run();
  std::cout << "wasSuccessful = " << std::boolalpha << wasSuccessful << '\n';
  return !wasSuccessful;
}
