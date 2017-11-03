#include <iostream>
#include <iomanip>
#include <future>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include "Message.h"
#include "CaptureDevice.h"
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

class CaptureTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(CaptureTest);
    CPPUNIT_TEST(capture);
    CPPUNIT_TEST_SUITE_END();
public:
    void capture() {
        std::stringstream ss;
        {
        SafeQueue<Message> captureIn;
        CaptureDevice cap{captureIn};
        captureIn.push(shortMsg);
        captureIn.push(longMsg);
        cap.verbosity(true);
        cap.hold();
        std::thread capThread{&CaptureDevice::run, &cap, &std::cin, &ss};
        // allow the capture thread time to process messages
        std::this_thread::sleep_for(std::chrono::seconds{1});
        cap.releaseHold();
        capThread.join();
        }
        int i = 4;
        std::string got{ss.str()};
        // find first timestamp
        auto ts1loc = 15*4;
        std::cout << "ts1loc = " << std::hex << ts1loc << '\n';
        auto ts2loc = 38*4;
        std::cout << "ts2loc = " << std::hex << ts2loc << '\n';
        // insert fixed timestamps for comparison
        for (auto ch : got) {
            std::cout << "\\x" << std::setw(2) << std::setfill('0') << std::hex << (static_cast<unsigned>(ch) & 0xffu);
            if (--i == 0) {
                std::cout << '\n';
                i = 4;
            }
        }
        std::cout << '\n';
        got.replace(ts1loc, 8, ts1);
        got.replace(ts2loc, 8, ts2);
        for (auto ch : got) {
            std::cout << "\\x" << std::setw(2) << std::setfill('0') << std::hex << (static_cast<unsigned>(ch) & 0xffu);
            if (--i == 0) {
                std::cout << '\n';
                i = 4;
            }
        }
        std::cout << '\n';
        std::cout << "desired len=" << desired.length() << '\n';
        std::cout << "got len=" << got.length() << '\n';

        CPPUNIT_ASSERT(got == desired);
    }
private:
    static const Message shortMsg;
    static const Message longMsg;
    static const std::string desired;
    static const std::string ts1;
    static const std::string ts2;
};

const std::string CaptureTest::ts1{"\xd2\x5b\x05\x00\x8d\x36\xa3\x37",8};
const std::string CaptureTest::ts2{"\xd2\x5b\x05\x00\x94\x36\xa3\x37",8};

const Message CaptureTest::shortMsg{0xc0, 0x31, 0x01, 0xe3, 0xa1, 0xbc, 0x01, 0xff, 0x0f, 0xfe, 0xff, 0x59, 0x19, 0x00, 0x05, 0x15, 0x01, 0x00, 0xd7, 0x82, 0xf0, 0x00, 0x3f, 0x1f, 0xa0, 0x06, 0x88, 0xfa, 0x05, 0x0a, 0x10, 0x01, 0x01, 0x05, 0x04, 0xe8, 0x03, 0x00, 0x00, 0x23, 0x0e, 0x05, 0x45, 0x50, 0x52, 0x49, 0x2d, 0x57, 0x69, 0x53, 0x55, 0x4e, 0x2d, 0x46, 0x41, 0x4e, 0xb4, 0xa1, 0x0a, 0x10, 0xc0};

const Message CaptureTest::longMsg{0xc0, 0x31, 0x09, 0xe3, 0xa1, 0xbc, 0x01, 0xff, 0x0f, 0xfe, 0xff, 0x59, 0x19, 0x00, 0x0e, 0x05, 0x31, 0x00, 0x00, 0x01, 0x05, 0x15, 0x01, 0x02, 0x2b, 0x00, 0x00, 0x07, 0x15, 0x02, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x3c, 0xa0, 0x06, 0x88, 0xfa, 0x05, 0x0a, 0x10, 0x01, 0x01, 0x0c, 0x90, 0x00, 0xa6, 0x0e, 0x00, 0x2a, 0x00, 0x78, 0x05, 0x0a, 0x10, 0x01, 0x01, 0x02, 0x06, 0x01, 0x00, 0x20, 0x07, 0x21, 0x43, 0x65, 0x87, 0x00, 0x00, 0x00, 0x00, 0x22, 0x43, 0x65, 0x87, 0x00, 0x00, 0x00, 0x00, 0x23, 0x43, 0x65, 0x87, 0x00, 0x00, 0x00, 0x00, 0x24, 0x43, 0x65, 0x87, 0x00, 0x00, 0x00, 0x00, 0x2e, 0xa2, 0x2a, 0x33, 0x23, 0xe7, 0x8f, 0xf4, 0x34, 0x61, 0x00, 0x00, 0xc0};  

const std::string CaptureTest::desired{
"\x0a\x0d\x0d\x0a" 
"\x1c\x00\x00\x00" 
"\x4d\x3c\x2b\x1a" 
"\x01\x00\x00\x00" 
"\xff\xff\xff\xff" 
"\xff\xff\xff\xff" 
"\x1c\x00\x00\x00" 
"\x01\x00\x00\x00" 
"\x14\x00\x00\x00" 
"\xe6\x00\x00\x00" 
"\xff\xff\x00\x00" 
"\x14\x00\x00\x00" 
"\x06\x00\x00\x00" 
"\x5c\x00\x00\x00" 
"\x00\x00\x00\x00" 
"\xd2\x5b\x05\x00" 
"\x8d\x36\xa3\x37" 
"\x3c\x00\x00\x00" 
"\x3c\x00\x00\x00" 
"\x31\x01\xe3\xa1" 
"\xbc\x01\xff\x0f" 
"\xfe\xff\x59\x19" 
"\x00\x05\x15\x01" 
"\x00\xd7\x82\xf0" 
"\x00\x3f\x1f\xa0" 
"\x06\x88\xfa\x05" 
"\x0a\x10\x01\x01" 
"\x05\x04\xe8\x03" 
"\x00\x00\x23\x0e" 
"\x05\x45\x50\x52" 
"\x49\x2d\x57\x69" 
"\x53\x55\x4e\x2d" 
"\x46\x41\x4e\xb4" 
"\xa1\x0a\x10\xc0" 
"\x5c\x00\x00\x00" 
"\x06\x00\x00\x00" 
"\x90\x00\x00\x00" 
"\x00\x00\x00\x00" 
"\xd2\x5b\x05\x00" 
"\x94\x36\xa3\x37" 
"\x70\x00\x00\x00" 
"\x70\x00\x00\x00" 
"\x31\x09\xe3\xa1" 
"\xbc\x01\xff\x0f" 
"\xfe\xff\x59\x19" 
"\x00\x0e\x05\x31" 
"\x00\x00\x01\x05" 
"\x15\x01\x02\x2b" 
"\x00\x00\x07\x15" 
"\x02\x07\x00\x00" 
"\x00\x00\x00\x00" 
"\x3f\x3c\xa0\x06" 
"\x88\xfa\x05\x0a" 
"\x10\x01\x01\x0c" 
"\x90\x00\xa6\x0e" 
"\x00\x2a\x00\x78" 
"\x05\x0a\x10\x01" 
"\x01\x02\x06\x01" 
"\x00\x20\x07\x21" 
"\x43\x65\x87\x00" 
"\x00\x00\x00\x22" 
"\x43\x65\x87\x00" 
"\x00\x00\x00\x23" 
"\x43\x65\x87\x00" 
"\x00\x00\x00\x24" 
"\x43\x65\x87\x00" 
"\x00\x00\x00\x2e" 
"\xa2\x2a\x33\x23" 
"\xe7\x8f\xf4\x34" 
"\x61\x00\x00\xc0" 
"\x90\x00\x00\x00"
,0x11c
};

/*
received: c03101e3a1bc01ff0ffeff59190005150100d782f0003f1fa00688fa050a1001010504e8030000230e05455052492d576953554e2d46414eb4a10a10c0
received: c03109e3a1bc01ff0ffeff5919000e0531000001051501022b0000071502070000000000003f3ca00688fa050a1001010c9000a60e002a0078050a10010102060100200721436587000000002243658700000000234365870000000024436587000000002ea22a3323e78ff434610000c0 
*/

CPPUNIT_TEST_SUITE_REGISTRATION(CaptureTest);

int main()
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run();
  std::cout << "wasSuccessful = " << std::boolalpha << wasSuccessful << '\n';
  return !wasSuccessful;
}
