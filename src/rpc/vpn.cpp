// Copytight (c) 2018-2019 The Veles Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <rpc/server.h>

#include <chainparams.h>
#include <clientversion.h>
#include <core_io.h>
#include <validation.h>
#include <net.h>
#include <net_processing.h>
#include <netbase.h>
#include <policy/policy.h>
#include <rpc/protocol.h>
#include <sync.h>
#include <timedata.h>
#include <ui_interface.h>
#include <util.h>
#include <utilstrencodings.h>
#include <version.h>
#include <warnings.h>

#include <univalue.h>

/*
static UniValue getvpninfo(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0)
        throw std::runtime_error(
            "getvpninfo\n"
            "\nReturns data about Veles dVPN network as a json array.\n"
            "\n* This method is currently EXPERIMENTAL and the output format might change at any time *\n"
            "\nExamples:\n"
            + HelpExampleCli("getvpninfo", "")
            + HelpExampleRpc("getvpninfo", "")
        );

    throw JSONRPCError(RPC_INTERNAL_ERROR, "Error: dVPN functionality has not been activated yet");
}

static UniValue getvpntotals(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0)
        throw std::runtime_error(
            "getvpntotals\n"
            "\nReturns information about dVPN traffic, including bytes in, bytes out,\n"
            "and current time.\n"
            "\n* This method is currently EXPERIMENTAL and the output format might change at any time *\n"
            "\nExamples:\n"
            + HelpExampleCli("getvpntotals", "")
            + HelpExampleRpc("getvpntotals", "")
        );

    UniValue obj(UniValue::VOBJ);
    obj.pushKV("totalbytesrecv", 0);
    obj.pushKV("totalbytessent", 0);
    obj.pushKV("timemillis", GetTimeMillis());
    return obj;
}
*/
/*
// Preparation
static const CRPCCommand commands[] =
{ //  category              name                      actor (function)         argNames
  //  --------------------- ------------------------  -----------------------  ----------
   // { "vpn",                "getvpninfo",             &getvpninfo,             {} },
   // { "vpn",                "getvpntotals",           &getvpntotals,           {} },
};

void RegisterVpnRPCCommands(CRPCTable &t)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        t.appendCommand(commands[vcidx].name, &commands[vcidx]);
}
*/
