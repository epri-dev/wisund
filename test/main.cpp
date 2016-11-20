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
#include "Console.h"

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

class ConsoleTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ConsoleTest);
    CPPUNIT_TEST(testBasic);
    CPPUNIT_TEST(testReplies);
    CPPUNIT_TEST(testRun);
    CPPUNIT_TEST_SUITE_END();
public:
    void testBasic() {
        std::stringstream cmds{"help\ndiag 2\nstate\nphy 3\nrestart\n"};
        CPPUNIT_ASSERT(con != nullptr);
        CPPUNIT_ASSERT(con->runTx(&cmds) == 0);
        Message m{0};
        CPPUNIT_ASSERT(output.try_pop(m));
        Message diag2{0x06, 0x21, 0x02};
        CPPUNIT_ASSERT(m == diag2);
        CPPUNIT_ASSERT(output.try_pop(m));
        Message state{0x06, 0x20};
        CPPUNIT_ASSERT(m == state);
        CPPUNIT_ASSERT(output.try_pop(m));
        Message phy3{0x06, 0x04, 0x03};
        CPPUNIT_ASSERT(m == phy3);
        CPPUNIT_ASSERT(output.try_pop(m));
        Message restart{0xff};
        CPPUNIT_ASSERT(m == restart);
        for (Message m{0}; output.try_pop(m); ) {
            std::cout << "msg: " << m << '\n';
        }
    }
    void testReplies() {
        std::stringstream reply;
        CPPUNIT_ASSERT(con != nullptr);
        Message diag2reply{0x21, 0x30, 0x55, 0xfe, 0x45};
        input.push(diag2reply);
        std::future<int> result = std::async(std::launch::async, &Console::runRx, con, &reply);
    }
    void testRun() {
        std::stringstream cmds{"diag 2\nrestart\n"};
        std::stringstream reply;
        CPPUNIT_ASSERT(con != nullptr);
        Message diag2reply{0x21, 0xde, 0xad, 0xbe, 0xef};
        input.push(diag2reply);
        CPPUNIT_ASSERT(con->run(&cmds, &reply) == 0);
        std::cout << reply.str();
    }
    void setUp() {
        con = new Console(input, output);
    }
    void tearDown() {
        delete con;
    }
private:
    Console *con;
    SafeQueue<Message> input, output; 
};

CPPUNIT_TEST_SUITE_REGISTRATION(MessageTest);
CPPUNIT_TEST_SUITE_REGISTRATION(ConsoleTest);

int main()
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run();
  std::cout << "wasSuccessful = " << std::boolalpha << wasSuccessful << '\n';
  return !wasSuccessful;
}
