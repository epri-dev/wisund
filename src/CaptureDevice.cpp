/** 
 *  \file CaptureDevice.cpp
 *  \brief Implementation of the CaptureDevice class
 */
#include "CaptureDevice.h"
#include "pcapng.h"
#include <iostream>

CaptureDevice::CaptureDevice(SafeQueue<Message> &input) :
    SinkDevice{input },
    m_verbose{false}
{}

CaptureDevice::~CaptureDevice() = default;

static void createHeader(std::ostream &out)
{
    using namespace pcapng;
    SHB shb;
    shb.write(out);
    IDB idb;
    idb.write(out);
}

int CaptureDevice::run(std::istream *in, std::ostream *out)
{
    using namespace pcapng;
    in = in;
    createHeader(*out);
    Message m{};
    while (wantHold()) {
        wait_and_pop(m);
        if (m_verbose) {
            std::cout << "CaptureDevice  raw msg: " << m << "\n";
        }
        if (m.size() > 1) {
            EPB ebp;
            ebp.write(*out, &m[1], m.size()-1);
            // flush each packet to allow live update via pipe
            out->flush();
        }
    }
    return 0;
}

bool CaptureDevice::verbosity(bool verbose) {
    std::swap(verbose, m_verbose);
    return verbose;
}
