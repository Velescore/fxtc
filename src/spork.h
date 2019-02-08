// Copyright (c) 2014-2017 The Dash Core developers
// Copyright (c) 2018 FXTC developers
// Copyright (c) 2018-2019 The Veles Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPORK_H
#define SPORK_H

#include "hash.h"
#include "net.h"
#include "utilstrencodings.h"

// FXTC BEGIN
class CSporkDB;

/** Global variable that points to the spork database (protected by cs_main) */
extern std::unique_ptr<CSporkDB> pSporkDB;
//FXTC END

class CSporkMessage;
class CSporkManager;

/*
    Don't ever reuse these IDs for other sporks
    - This would result in old clients getting confused about which spork is for what
*/
static const int SPORK_START                                            = 10001;
static const int SPORK_END                                              = 10013;
// FXTC BEGIN
static const int SPORK_FXTC_START                                    = 94680010;
static const int SPORK_FXTC_END                                      = 94680031;
// FXTC END
// VELES BEGIN
static const int SPORK_VELES_START                                    = 94690010;
static const int SPORK_VELES_END                                      = 94690051;
// VELES END

static const int SPORK_2_INSTANTSEND_ENABLED                            = 10001;
static const int SPORK_3_INSTANTSEND_BLOCK_FILTERING                    = 10002;
static const int SPORK_5_INSTANTSEND_MAX_VALUE                          = 10004;
static const int SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT                 = 10007;
static const int SPORK_9_SUPERBLOCKS_ENABLED                            = 10008;
static const int SPORK_10_MASTERNODE_PAY_UPDATED_NODES                  = 10009;
static const int SPORK_12_RECONSIDER_BLOCKS                             = 10011;
static const int SPORK_13_OLD_SUPERBLOCK_FLAG                           = 10012;
static const int SPORK_14_REQUIRE_SENTINEL_FLAG                         = 10013;

// FXTC BEGIN
static const int SPORK_FXTC_01_HANDBRAKE_HEIGHT                      = 94680010;
static const int SPORK_FXTC_01_HANDBRAKE_FORCE_SHA256D               = 94680011;
static const int SPORK_FXTC_01_HANDBRAKE_FORCE_SCRYPT                = 94680012;
static const int SPORK_FXTC_01_HANDBRAKE_FORCE_NIST5                 = 94680013;
static const int SPORK_FXTC_01_HANDBRAKE_FORCE_LYRA2Z                = 94680014;
static const int SPORK_FXTC_01_HANDBRAKE_FORCE_X11                   = 94680015;
static const int SPORK_FXTC_01_HANDBRAKE_FORCE_X16R                  = 94680016;
static const int SPORK_FXTC_02_IGNORE_SLIGHTLY_HIGHER_COINBASE       = 94680021;
static const int SPORK_VELES_02_OPTIONAL_DEV_REWARD_START            = 94680022;    // VELES EDIT: rename
static const int SPORK_VELES_02_OPTIONAL_DEV_REWARD_VALUE            = 94680023;    // VELES EDIT: rename
static const int SPORK_FXTC_02_IGNORE_MASTERNODE_REWARD_VALUE        = 94680024;
static const int SPORK_FXTC_02_IGNORE_MASTERNODE_REWARD_PAYEE        = 94680025;
static const int SPORK_FXTC_03_BLOCK_REWARD_SMOOTH_HALVING_START     = 94680031;
// FXTC END

// VELES BEGIN
static const int SPORK_VELES_01_FXTC_CHAIN_START                       = 94690010;
static const int SPORK_VELES_02_UNLIMITED_BLOCK_SUBSIDY_START          = 94690020;
static const int SPORK_VELES_03_RECALCULATE_HALVING_PARAMETERS         = 94690030;
static const int SPORK_VELES_04_REWARD_UPGRADE_ALPHA_START             = 94690031;  // VCIP01
static const int SPORK_VELES_05A_ADJUST_COST_FACTOR_START              = 94690032;
static const int SPORK_VELES_05A_ADJUST_COST_FACTOR_SHA256D            = 94690033;
static const int SPORK_VELES_05A_ADJUST_COST_FACTOR_SCRYPT             = 94690034;
static const int SPORK_VELES_05A_ADJUST_COST_FACTOR_NIST5              = 94690035;
static const int SPORK_VELES_05A_ADJUST_COST_FACTOR_LYRA2Z             = 94690036;
static const int SPORK_VELES_05A_ADJUST_COST_FACTOR_X11                = 94690037;
static const int SPORK_VELES_05A_ADJUST_COST_FACTOR_X16R               = 94690038;
static const int SPORK_VELES_05B_ADJUST_COST_FACTOR_START              = 94690039;
static const int SPORK_VELES_05B_ADJUST_COST_FACTOR_SHA256D            = 94690040;
static const int SPORK_VELES_05B_ADJUST_COST_FACTOR_SCRYPT             = 94690041;
static const int SPORK_VELES_05B_ADJUST_COST_FACTOR_NIST5              = 94690042;
static const int SPORK_VELES_05B_ADJUST_COST_FACTOR_LYRA2Z             = 94690043;
static const int SPORK_VELES_05B_ADJUST_COST_FACTOR_X11                = 94690044;
static const int SPORK_VELES_05B_ADJUST_COST_FACTOR_X16R               = 94690045;
static const int SPORK_VELES_06A_DYNAMIC_REWARD_BOOST1_START           = 94690046;
static const int SPORK_VELES_06A_DYNAMIC_REWARD_BOOST1_FACTOR          = 94690047;
static const int SPORK_VELES_06B_DYNAMIC_REWARD_BOOST2_START           = 94690048;
static const int SPORK_VELES_06B_DYNAMIC_REWARD_BOOST2_FACTOR          = 94690049;
static const int SPORK_VELES_06C_DYNAMIC_REWARD_BOOST3_START           = 94690050;
static const int SPORK_VELES_06C_DYNAMIC_REWARD_BOOST3_FACTOR          = 94690051;
// VELES END

static const int64_t SPORK_2_INSTANTSEND_ENABLED_DEFAULT                = 0;            // ON
static const int64_t SPORK_3_INSTANTSEND_BLOCK_FILTERING_DEFAULT        = 0;            // ON
static const int64_t SPORK_5_INSTANTSEND_MAX_VALUE_DEFAULT              = 1000;         // 1000 Veles
static const int64_t SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT_DEFAULT     = 1539964282;   // OFF
static const int64_t SPORK_9_SUPERBLOCKS_ENABLED_DEFAULT                = 4070908800ULL;// OFF
static const int64_t SPORK_10_MASTERNODE_PAY_UPDATED_NODES_DEFAULT      = 4070908800ULL;// OFF
static const int64_t SPORK_12_RECONSIDER_BLOCKS_DEFAULT                 = 0;            // 0 BLOCKS
static const int64_t SPORK_13_OLD_SUPERBLOCK_FLAG_DEFAULT               = 4070908800ULL;// OFF
static const int64_t SPORK_14_REQUIRE_SENTINEL_FLAG_DEFAULT             = 4070908800ULL;// OFF

// FXTC BEGIN
static const int64_t SPORK_FXTC_01_HANDBRAKE_HEIGHT_DEFAULT             = 4070908800ULL;// OFF
static const int64_t SPORK_FXTC_01_HANDBRAKE_FORCE_SHA256D_DEFAULT      = 1;            // 1x
static const int64_t SPORK_FXTC_01_HANDBRAKE_FORCE_SCRYPT_DEFAULT       = 1;            // 1x
static const int64_t SPORK_FXTC_01_HANDBRAKE_FORCE_NIST5_DEFAULT        = 1;            // 1x
static const int64_t SPORK_FXTC_01_HANDBRAKE_FORCE_LYRA2Z_DEFAULT       = 1;            // 1x
static const int64_t SPORK_FXTC_01_HANDBRAKE_FORCE_X11_DEFAULT          = 1;            // 1x
static const int64_t SPORK_FXTC_01_HANDBRAKE_FORCE_X16R_DEFAULT         = 1;            // 1x
static const int64_t SPORK_FXTC_02_IGNORE_SLIGHTLY_HIGHER_COINBASE_DEFAULT  = 4070908800ULL;// OFF
static const int64_t SPORK_VELES_02_OPTIONAL_DEV_REWARD_START_DEFAULT       = 1;            // On since VCIP01
static const int64_t SPORK_VELES_02_OPTIONAL_DEV_REWARD_VALUE_DEFAULT       = 1;            // On since VCIP01
static const int64_t SPORK_FXTC_02_IGNORE_MASTERNODE_REWARD_VALUE_DEFAULT   = 4070908800ULL;// OFF
static const int64_t SPORK_FXTC_02_IGNORE_MASTERNODE_REWARD_PAYEE_DEFAULT   = 4070908800ULL;// OFF
static const int64_t SPORK_FXTC_03_BLOCK_REWARD_SMOOTH_HALVING_START_DEFAULT  = 4070908800ULL;// OFF
// FXTC END

// VELES BEGIN
static const int64_t SPORK_VELES_01_FXTC_CHAIN_START_DEFAULT                  = 11;// OFF
static const int64_t SPORK_VELES_02_UNLIMITED_BLOCK_SUBSIDY_START_DEFAULT     = 4070908800ULL;  // OFF
static const int64_t SPORK_VELES_03_RECALCULATE_HALVING_PARAMETERS_DEFAULT    = 54000;
static const int64_t SPORK_VELES_04_REWARD_UPGRADE_ALPHA_START_DEFAULT        = 50000;          // VCIP01
static const int64_t SPORK_VELES_05A_ADJUST_COST_FACTOR_START_DEFAULT         = 4070908800ULL;  // OFF
static const int64_t SPORK_VELES_05A_ADJUST_COST_FACTOR_SHA256D_DEFAULT       = 900;
static const int64_t SPORK_VELES_05A_ADJUST_COST_FACTOR_SCRYPT_DEFAULT        = 250;
static const int64_t SPORK_VELES_05A_ADJUST_COST_FACTOR_NIST5_DEFAULT         = 100;
static const int64_t SPORK_VELES_05A_ADJUST_COST_FACTOR_LYRA2Z_DEFAULT        = 50;
static const int64_t SPORK_VELES_05A_ADJUST_COST_FACTOR_X11_DEFAULT           = 125;
static const int64_t SPORK_VELES_05A_ADJUST_COST_FACTOR_X16R_DEFAULT          = 150;
static const int64_t SPORK_VELES_05B_ADJUST_COST_FACTOR_START_DEFAULT         = 4070908800ULL;  // OFF
static const int64_t SPORK_VELES_05B_ADJUST_COST_FACTOR_SHA256D_DEFAULT       = 900;
static const int64_t SPORK_VELES_05B_ADJUST_COST_FACTOR_SCRYPT_DEFAULT        = 250;
static const int64_t SPORK_VELES_05B_ADJUST_COST_FACTOR_NIST5_DEFAULT         = 100;
static const int64_t SPORK_VELES_05B_ADJUST_COST_FACTOR_LYRA2Z_DEFAULT        = 50;
static const int64_t SPORK_VELES_05B_ADJUST_COST_FACTOR_X11_DEFAULT           = 125;
static const int64_t SPORK_VELES_05B_ADJUST_COST_FACTOR_X16R_DEFAULT          = 150;
static const int64_t SPORK_VELES_06A_DYNAMIC_REWARD_BOOST1_START_DEFAULT      = 4070908800ULL;  // OFF
static const int64_t SPORK_VELES_06A_DYNAMIC_REWARD_BOOST1_FACTOR_DEFAULT     = 100;
static const int64_t SPORK_VELES_06B_DYNAMIC_REWARD_BOOST2_START_DEFAULT      = 4070908800ULL;  // OFF
static const int64_t SPORK_VELES_06B_DYNAMIC_REWARD_BOOST2_FACTOR_DEFAULT     = 100;
static const int64_t SPORK_VELES_06C_DYNAMIC_REWARD_BOOST3_START_DEFAULT      = 4070908800ULL;  // OFF
static const int64_t SPORK_VELES_06C_DYNAMIC_REWARD_BOOST3_FACTOR_DEFAULT     = 100;
// VELES END

extern std::map<uint256, CSporkMessage> mapSporks;
extern CSporkManager sporkManager;

//
// Spork classes
// Keep track of all of the network spork settings
//

class CSporkMessage
{
private:
    std::vector<unsigned char> vchSig;

public:
    int nSporkID;
    int64_t nValue;
    int64_t nTimeSigned;

    CSporkMessage(int nSporkID, int64_t nValue, int64_t nTimeSigned) :
        nSporkID(nSporkID),
        nValue(nValue),
        nTimeSigned(nTimeSigned)
        {}

    CSporkMessage() :
        nSporkID(0),
        nValue(0),
        nTimeSigned(0)
        {}


    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(nSporkID);
        READWRITE(nValue);
        READWRITE(nTimeSigned);
        READWRITE(vchSig);
    }

    uint256 GetHash() const
    {
        CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
        ss << nSporkID;
        ss << nValue;
        ss << nTimeSigned;
        return ss.GetHash();
    }

    bool Sign(std::string strSignKey);
    bool CheckSignature();
    void Relay(CConnman& connman);
};


class CSporkManager
{
private:
    std::vector<unsigned char> vchSig;
    std::string strMasterPrivKey;
    std::map<int, CSporkMessage> mapSporksActive;

public:

    CSporkManager() {}

    // FXTC BEGIN
    void LoadSporksFromDB();
    // FXTC END
    void ProcessSpork(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman& connman);
    void ExecuteSpork(int nSporkID, int nValue);
    bool UpdateSpork(int nSporkID, int64_t nValue, CConnman& connman);

    bool IsSporkActive(int nSporkID);
    int64_t GetSporkValue(int nSporkID);
    int GetSporkIDByName(std::string strName);
    std::string GetSporkNameByID(int nSporkID);

    bool SetPrivKey(std::string strPrivKey);
};

#endif
