#include <iostream>
#include <iomanip>
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

class ConsoleTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ConsoleTest);
    CPPUNIT_TEST(testBasic);
    CPPUNIT_TEST(testReplies);
    CPPUNIT_TEST(testMacReply);
    CPPUNIT_TEST(testRun);
    CPPUNIT_TEST(testNeighbors);
    CPPUNIT_TEST_SUITE_END();
public:
    void testBasic() {
        std::stringstream cmds{"help\ndiag 02\nstate\nphy 03\nrestart\n"};
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
        CPPUNIT_ASSERT(con->runRx(&reply) == 0);
    }
    void testMacReply() {
        std::stringstream reply;
        CPPUNIT_ASSERT(con != nullptr);
        Message macReply{0x24, 0x01, 0x02, 0xf3, 0xe4, 0xd5, 0xc6, 0xb7, 0xa8};
        std::string desired{"{ \"mac\":\"01:02:f3:e4:d5:c6:b7:a8\" }\n"};
        input.push(macReply);
        CPPUNIT_ASSERT(con->runRx(&reply) == 0);
        CPPUNIT_ASSERT(reply.str() == desired);
    }

    void testRun() {
        std::stringstream cmds{"diag 02\n"};
        std::stringstream reply;
        CPPUNIT_ASSERT(con != nullptr);
        Message diag2reply{
            0x21,0x02,0x00,0x00,0x00,0x00,0x5f,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,
            0x00,0x00,0x5f,0x00,0x00,0x00,0x42,0x00,
            0x00,0x00,0x2e,0x00,0x00,0x00,0x2f,0x00,
            0x00,0x00,0x30,0x00,0x00,0x00,0x30,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00};
        std::string desired{R"x({ "iecounters": { "fcie":0, "uttie":95, "rslie":0, "btie":48, "usie":95, "bsie":66, "panie":46, "netnameie":47, "panverie":48, "gtkhashie":48, "mpie":0, "mhdsie":0, "vhie":0, "vpie":0 } }
)x"};
        input.push(diag2reply);
        CPPUNIT_ASSERT(con->run(&cmds, &reply) == 0);
        CPPUNIT_ASSERT(reply.str() == desired);
    }

    void testNeighbors() {
        std::stringstream cmds{"neighbors\n"};
        std::stringstream reply;
        CPPUNIT_ASSERT(con != nullptr);
        Message neighborsreply{
            0x23,0x02,
                0x00,0x01,0x7e,0x02,0x4e,0x00,
                0x00,0x19,0x59,0xff,0xfe,0x0f,0xff,0x02,
                0x01,0x01,0x7f,0x02,0x4e,0x00,
                0x00,0x19,0x59,0xff,0xfe,0x0f,0xff,0x03 };
        std::string desired{R"({ "neighbors": [ { "index":0, "validated":1, "timestamp":5112446, "mac":"00:19:59:ff:fe:0f:ff:02"}, { "index":1, "validated":1, "timestamp":5112447, "mac":"00:19:59:ff:fe:0f:ff:03"} ] }
)"};
        input.push(neighborsreply);
        CPPUNIT_ASSERT(con->run(&cmds, &reply) == 0);
        CPPUNIT_ASSERT(reply.str() == desired);
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
