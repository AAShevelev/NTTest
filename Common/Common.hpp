#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>

static short port = 5555;

namespace Requests
{
    static std::string Registration = "Reg";
    static std::string Buy = "Buy";
    static std::string Sell = "Sell";
    static std::string Balance = "Bal";
    static std::string Active = "Act";
    static std::string Reject = "Rej";
    static std::string History = "His";
}

#endif //CLIENSERVERECN_COMMON_HPP
