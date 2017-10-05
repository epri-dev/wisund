/** 
 *  \file CaptureDevice.cpp
 *  \brief Implementation of the CaptureDevice class
 */
#include "CaptureDevice.h"
#include <iostream>

CaptureDevice::CaptureDevice(SafeQueue<Message> &input) :
    SinkDevice{input },
    m_verbose{false}
{}

CaptureDevice::~CaptureDevice() = default;

int CaptureDevice::run(std::istream *in, std::ostream *out)
{
    in = in;
    out = out;
    Message m{};
    while (wantHold()) {
        wait_and_pop(m);
        if (m_verbose) {
            std::cout << "CaptureDevice  raw msg: " << m << "\n";
        }
        // TODO: other processing here
    }
    return 0;
}

bool CaptureDevice::verbosity(bool verbose) {
    std::swap(verbose, m_verbose);
    return verbose;
}
