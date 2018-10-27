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
#include "SinkDevice.h"
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

class TestSinkDevice : public SinkDevice {
public:
    int run(std::istream *in, std::ostream *out) { 
        in = in;
        Message m{};
        while (wantHold()) {
            wait_and_pop(m);
            *out << m << '\n';
        }
        return 0; 
    }
};

class SinkDeviceTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SinkDeviceTest);
    CPPUNIT_TEST(sink);
    CPPUNIT_TEST(testSinkHold);
    CPPUNIT_TEST(simpleMessage);
    CPPUNIT_TEST(testTry_popempty);
    CPPUNIT_TEST(testTry_popmsg);
    CPPUNIT_TEST_SUITE_END();
public:
    /* 
     * simple test creates device which should immediately return because 
     * wantHold() is not asserted and there are no messages
     */
    void sink() {
        std::stringstream ss;
        TestSinkDevice sinker{};
        std::thread capThread{&TestSinkDevice::run, &sinker, &std::cin, &ss};
        CPPUNIT_ASSERT(sinker.wantHold() == false);
        capThread.join();
    }
    /* 
     * simple test creates device which will wait because 
     * wantHold() is asserted, then released and there are no messages
     */
    void testSinkHold() {
        std::stringstream ss;
        TestSinkDevice sinker{};
        sinker.hold();
        std::thread capThread{&TestSinkDevice::run, &sinker, &std::cin, &ss};
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        CPPUNIT_ASSERT(sinker.wantHold() == true);
        CPPUNIT_ASSERT(sinker.more() == false);
        sinker.releaseHold();
        capThread.join();
    }

    /*
     * create a device, put it in verbose mode and then put a message
     * in the input queue which should then print
     */
    void simpleMessage() {
        std::stringstream ss;
        TestSinkDevice sinker{};
        sinker.hold();
        std::thread capThread{&TestSinkDevice::run, &sinker, &std::cin, &ss};
        Message msg{0x41,0x42,0x43,0x44,0x45,0x46,0x47};
        sinker.in().push(msg);
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        CPPUNIT_ASSERT(sinker.wantHold() == true);
        CPPUNIT_ASSERT(sinker.more() == false);
        sinker.releaseHold();
        capThread.join();
        CPPUNIT_ASSERT(ss.str() == "41424344454647\n\n");
    }

    /*
     * create a device and execute try_pop
     * which should immediately exit with false
     */
    void testTry_popempty() {
        TestSinkDevice sinker{};
        Message m{};
        CPPUNIT_ASSERT(!sinker.try_pop(m));
    }

    /*
     * create a device, push a message and execute try_pop
     * which should immediately exit with true
     */
    void testTry_popmsg() {
        TestSinkDevice sinker{};
        // push arbitrary message
        Message msg{0x81,0x82,0xff,0x99};
        sinker.in().push(msg);
        Message m{};
        CPPUNIT_ASSERT(sinker.try_pop(m));
        CPPUNIT_ASSERT(m == msg);
    }
};


CPPUNIT_TEST_SUITE_REGISTRATION(SinkDeviceTest);

int main()
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run();
  std::cout << "wasSuccessful = " << std::boolalpha << wasSuccessful << '\n';
  return !wasSuccessful;
}
