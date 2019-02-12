// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef FXTC_POW_H
#define FXTC_POW_H

#include <consensus/params.h>

#include <stdint.h>

class CBlockHeader;
class CBlockIndex;
class uint256;

// VELES BEGIN
// Define difficulty retarget algorithms
enum DiffMode {
    DIFF_DEFAULT = 0, // Default to invalid 0
    DIFF_BTC     = 1, // Retarget every x blocks (Bitcoin style)
    DIFF_KGW     = 2, // Retarget using Kimoto Gravity Well
    DIFF_DGW     = 3, // Retarget using Dark Gravity Wave v3
};

unsigned int GetNextWorkRequiredVLS(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params&);
// VELES END

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params&);
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params&);

/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params&);

// FXTC BEGIN
unsigned int GetHandbrakeForce(int32_t nVersion, int nHeight);
// FXTC END

#endif // FXTC_POW_H
