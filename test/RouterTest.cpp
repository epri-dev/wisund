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
#include "Router.h"
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

class TestDevice : public Device {
public:
    TestDevice(SafeQueue<Message> &output) :
        Device(&output) 
    {}
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

class RouterTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(RouterTest);
    CPPUNIT_TEST(router);
    CPPUNIT_TEST(nullMessage);
    CPPUNIT_TEST(unsourcedMessage);
    CPPUNIT_TEST(bogusMessageSource);
    CPPUNIT_TEST(routeMessage);
    CPPUNIT_TEST_SUITE_END();
public:
    void router() {
        Router rtr;
        rtr.run(&std::cin, &std::cout);
    }
    void nullMessage() {
        Router rtr;
        rtr.push(Message{});
        rtr.run(&std::cin, &std::cout);
    }
    void unsourcedMessage() {
        std::stringstream ss;
        Router rtr;
        rtr.verbosity(true);
        rtr.hold();
        Message msg{0x81,0x82};
        CPPUNIT_ASSERT(msg.source == nullptr);
        rtr.in().push(msg);
        CPPUNIT_ASSERT_THROW(rtr.run(&std::cin, &ss), std::runtime_error);
    }
    void bogusMessageSource() {
        std::stringstream ss;
        Router rtr;
        rtr.verbosity(true);
        rtr.hold();
        Message msg{0x81,0x82};
        // arbitrary but bogus source value
        msg.setSource(reinterpret_cast<void *>(9));  
        rtr.in().push(msg);
        std::thread rtrThread{&Router::run, &rtr, &std::cin, &ss};
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        rtr.releaseHold();
        rtrThread.join();
        // should silently discard message
        // because there are no rules
        CPPUNIT_ASSERT(ss.str() == "");
    }
    void routeMessage() {
        std::stringstream ss;
        Router rtr;
        TestDevice td1{rtr.in()}; 
        TestDevice td2{rtr.in()};
        TestDevice td3{rtr.in()};
        rtr.addRule(&td1, &td2, &Message::isCap);
        rtr.addRule(&td1, &td3, &Message::isPlain);
        rtr.hold();
        Message plainmsg{0x61,0x82};
        plainmsg.setSource(&td1);
        Message capmsg{0x31,0x82};
        capmsg.setSource(&td1);
        rtr.in().push(capmsg);
        rtr.in().push(plainmsg);
        std::thread rtrThread{&Router::run, &rtr, &std::cin, &ss};
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        rtr.releaseHold();
        rtrThread.join();
        // should push message to td2
        Message reply{};
        CPPUNIT_ASSERT(td2.try_pop(reply));
        CPPUNIT_ASSERT(reply == capmsg);
        CPPUNIT_ASSERT(td3.try_pop(reply));
        CPPUNIT_ASSERT(reply == plainmsg);
    }

private:
};


CPPUNIT_TEST_SUITE_REGISTRATION(RouterTest);

int main()
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run();
  std::cout << "wasSuccessful = " << std::boolalpha << wasSuccessful << '\n';
  return !wasSuccessful;
}
