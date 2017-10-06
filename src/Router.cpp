/** 
 *  \file Router.cpp
 *  \brief Implementation of the Router class
 */
#include "Router.h"
#include <iostream>

Router::Router(SafeQueue<Message> &input, SafeQueue<Message> &output, SafeQueue<Message> &rawOutput, SafeQueue<Message> &capOutput) :
    Device{input, output},
    rawQ{rawOutput},
    capQ{capOutput},
    m_verbose{false}
{}

Router::~Router() = default;

int Router::run(std::istream *in, std::ostream *out)
{
    in = in;
    out = out;
    Message m{};
    while (wantHold()) {
        wait_and_pop(m);
        if (m.isRaw()) {
            if (m_verbose) {
                std::cout << "Router pushing raw msg: " << m << "\n";
            }
            rawQ.push(m);
        } else if (m.isCap()) {
            if (m_verbose) {
                std::cout << "Router pushing capture msg: " << m << "\n";
            }
            capQ.push(m);
        }else {
            if (m_verbose) {
                std::cout << "Router pushing msg: " << m << "\n";
            }
            push(m);
        }
    }
    return 0;
}

bool Router::verbosity(bool verbose) {
    std::swap(verbose, m_verbose);
    return verbose;
}
