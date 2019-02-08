// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2018 FXTC developers
// Copyright (c) 2018 The Veles Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

//#include <base58.h>
#include <clientversion.h>
#include <coins.h>
#include <consensus/consensus.h>
#include <core_io.h>
#include <key_io.h> // new
#include <keystore.h>
#include <policy/policy.h>
#include <policy/rbf.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <script/sign.h>
#include <univalue.h>
#include <util.h>
#include <utilmoneystr.h>
#include <utilstrencodings.h>
#include <memory>
#include <stdio.h>
#include <boost/algorithm/string.hpp>

#include <masternodeman.h>
#include <masternode-payments.h>

#include "validation.h"

static bool fVerboseOutput;
static std::map<std::string,UniValue> registers;
static const int CONTINUE_EXECUTION=-1;

// From validation.cpp
double ConvertBitsToDouble(unsigned int nBits);

static void SetupBlockTestTxArgs()
{
    // Basic options
    gArgs.AddArg("-?", "This help message", false, OptionsCategory::OPTIONS);
    gArgs.AddArg("-subsidy", "Calculates subsidy amount for a block with specified parameters.", false, OptionsCategory::OPTIONS);
    gArgs.AddArg("-verbose", "Output extra debug information", false, OptionsCategory::OPTIONS);
    SetupChainParamsBaseOptions();

    // Hidden
    gArgs.AddArg("-h", "", false, OptionsCategory::HIDDEN);
    gArgs.AddArg("-help", "", false, OptionsCategory::HIDDEN);
}

//
// This function returns either one of EXIT_ codes when it's expected to stop the process or
// CONTINUE_EXECUTION when it's expected to continue further.
//
static int AppInitBlockTest(int argc, char* argv[])
{
    std::string error;
    SetupBlockTestTxArgs();
    
    if (!gArgs.ParseParameters(argc, argv, error)) {
        fprintf(stderr, "Error parsing command line arguments: %s\n", error.c_str());
        return EXIT_FAILURE;
    }

    // Check for -testnet or -regtest parameter (Params() calls are only valid after this clause)
    try {
        SelectParams(gArgs.GetChainName());
    } catch (const std::exception& e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return EXIT_FAILURE;
    }

    fVerboseOutput = gArgs.GetBoolArg("-verbose", false);

    if (argc < 3 || HelpRequested(gArgs)) {
        // First part of help message is specific to this utility
        std::string strUsage = PACKAGE_NAME " veles-testblock utility version " + FormatFullVersion() + "\n\n" +
            "Usage:  veles-testblock [options] -subsidy [height] [bitsHex] [versionHex]\n" +
            "\n";
        strUsage += gArgs.GetHelpMessage();

        fprintf(stdout, "%s", strUsage.c_str());

        if (argc < 3) {
            fprintf(stderr, "Error: too few parameters\n");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    return CONTINUE_EXECUTION;
}

CAmount BlockTestGetSubsidy(int nHeight, uint32_t nBits, uint32_t nVersion)
{
    CBlockHeader *header = new CBlockHeader();
    header->nVersion = nVersion;
    header->nBits = nBits;
   // const CChainParams& chainparams = Params();

    return GetBlockSubsidy(nHeight, *header, Params().GetConsensus());
}

static void BlockTestPrintSubsidyParams(int nHeight, uint32_t nBits, uint32_t nVersion)
{
    const Consensus::Params& consensusParams = Params().GetConsensus();
    std::string algoMsg = " Algorithm: %s\n";
    HalvingParameters *halvingParams = GetSubsidyHalvingParameters(nHeight, consensusParams);
    CBlockHeader *header = new CBlockHeader();

    header->nVersion = nVersion;

    fprintf(stdout, "%s\n", "Input parameters:");
    fprintf(stdout, " Height: %i\n", nHeight);
    fprintf(stdout, " Bits: %04x (%i)\n", nBits, nBits);
    fprintf(stdout, " Version: %04x (%i)\n", nVersion, nVersion);

    switch (nVersion & ALGO_VERSION_MASK)
    {
        case ALGO_SHA256D: fprintf(stdout, algoMsg.c_str(), "Sha256d"); break;
        case ALGO_SCRYPT:  fprintf(stdout, algoMsg.c_str(), "Scrypt"); break;
        case ALGO_NIST5:   fprintf(stdout, algoMsg.c_str(), "Nist5"); break;
        case ALGO_LYRA2Z:  fprintf(stdout, algoMsg.c_str(), "Lyra2z"); break;
        case ALGO_X11:     fprintf(stdout, algoMsg.c_str(), "X11"); break;
        case ALGO_X16R:    fprintf(stdout, algoMsg.c_str(), "X16R"); break;
        default:           fprintf(stdout, algoMsg.c_str(), "* unknown *"); break;
    }

    fprintf(stdout, "\n%s\n", "Chain parameters:");
    fprintf(stdout, " Halvings interval: %i (%i already occured)\n", halvingParams->nHalvingInterval, halvingParams->nHalvingCount);
    fprintf(
        stdout, 
        (nHeight >= sporkManager.GetSporkValue(SPORK_VELES_04_REWARD_UPGRADE_ALPHA_START) + consensusParams.nSubsidyHalvingInterval)
            ? " First halving occured on block:    %i\n"
            : " First halving will occur on block: %i\n", 
        (int) sporkManager.GetSporkValue(SPORK_VELES_04_REWARD_UPGRADE_ALPHA_START) + consensusParams.nSubsidyHalvingInterval
        );
    fprintf(stdout, " Alpha reward algo cost factor:     x %.2f\n", GetBlockAlgoCostFactor(header, nHeight));
    fprintf(stdout, " Alpha reward multiplier:           x %i\n", consensusParams.nVlsRewardsAlphaMultiplier);
    fprintf(stdout, "\n%s\n", "Activated hard forks / sporks:");
    fprintf(
        stdout, 
        " FXTC chain start spork:           %s (block %i)\n", 
        (nHeight >= sporkManager.GetSporkValue(SPORK_VELES_01_FXTC_CHAIN_START)) ? "YES" : "NO",
        (int) sporkManager.GetSporkValue(SPORK_VELES_01_FXTC_CHAIN_START)
    );
    fprintf(
        stdout, 
        " Alpha reward upgrade hard fork:   %s (block %i)\n",
        (nHeight >= sporkManager.GetSporkValue(SPORK_VELES_04_REWARD_UPGRADE_ALPHA_START)) ? "YES" : "NO",
        (int) sporkManager.GetSporkValue(SPORK_VELES_04_REWARD_UPGRADE_ALPHA_START)
    );
    fprintf(
        stdout, 
        " Budget/superblock hard fork:      %s (block %i)\n", 
        (nHeight >= consensusParams.nBudgetPaymentsStartBlock) ? "YES" : "NO",
        consensusParams.nBudgetPaymentsStartBlock
    );
    /*
    fprintf(
        stdout, 
        " No subsidy halving spork:         %s\n", 
        (nHeight >= sporkManager.GetSporkValue(SPORK_VELES_03_RECALCULATE_HALVING_PARAMETERS)) ? "YES" : "NO"
    //    (int) sporkManager.GetSporkValue(SPORK_VELES_03_NO_SUBSIDY_HALVING_START)
    );
    fprintf(
        stdout, 
        " Smooth subsidy halving spork:     %s\n", 
        (nHeight >= sporkManager.GetSporkValue(SPORK_FXTC_03_BLOCK_REWARD_SMOOTH_HALVING_START)
            || sporkManager.GetSporkValue(SPORK_VELES_04_REWARD_UPGRADE_ALPHA_START))  ? "YES" : "NO"
    //    (int) sporkManager.GetSporkValue(SPORK_FXTC_03_BLOCK_REWARD_SMOOTH_HALVING_START)
    );
    */
    fprintf(
        stdout, 
        " Unlimited block subsidy spork:    %s\n", 
        (nHeight >= sporkManager.GetSporkValue(SPORK_VELES_02_UNLIMITED_BLOCK_SUBSIDY_START)) ? "YES" : "NO"
    //    (int) sporkManager.GetSporkValue(SPORK_VELES_02_UNLIMITED_BLOCK_SUBSIDY_START)
    );
}

static int CommandLineRawTx(int argc, char* argv[])
{
    while (argc > 1 && IsSwitchChar(argv[1][0])) {
        argc--;
        argv++;
    }
    std::vector<std::string> args = std::vector<std::string>(&argv[1], &argv[argc]);

    if (gArgs.GetBoolArg("-subsidy", false)) {
        if (args.size() < 3) {
            throw std::runtime_error("too few parameters (need at least block height and number of bits)");
        }
        // Debug output
        if (fVerboseOutput) {
            BlockTestPrintSubsidyParams(std::stoi(args[0]), (uint32_t) std::stoi(args[1], 0, 16), (uint32_t) std::stoi(args[2], 0, 16));
            fprintf(stdout, "%s", "\nCalculated subsidy amount VLS: ");
        }

        // Final reasult
        fprintf(stdout, "%.8f\n", (double) BlockTestGetSubsidy(
            std::stoi(args[0]), 
            (uint32_t) std::stoi(args[1], 0, 16), 
            (uint32_t) std::stoi(args[2], 0, 16)
            ) / COIN);

    } else {
        throw std::runtime_error("too few parameters"); // we shouldn't be here
    }

    return EXIT_FAILURE;
}

int main(int argc, char* argv[])
{
    SetupEnvironment();

    try {
        int ret = AppInitBlockTest(argc, argv);
        if (ret != CONTINUE_EXECUTION)
            return ret;
    }
    catch (const std::exception& e) {
        PrintExceptionContinue(&e, "AppInitBlockTest()");
        return EXIT_FAILURE;
    } catch (...) {
        PrintExceptionContinue(nullptr, "AppInitBlockTest()");
        return EXIT_FAILURE;
    }

    int ret = EXIT_FAILURE;
    try {
        ret = CommandLineRawTx(argc, argv);
    }
    catch (const std::exception& e) {
        PrintExceptionContinue(&e, "CommandLineRawTx()");
    } catch (...) {
        PrintExceptionContinue(nullptr, "CommandLineRawTx()");
    }
    return ret;
}
