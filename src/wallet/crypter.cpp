// Copyright (c) 2009-2018 The Bitcoin Core developers
// Copyright (c) 2014-2017 The Dash Core developers
// Copyright (c) 2018 FXTC developers
// Copyright (c) 2018-2019 The Veles Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <wallet/crypter.h>

#include <crypto/aes.h>
#include <crypto/sha512.h>
#include <script/script.h>
#include <script/standard.h>
#include <util/system.h>

#include <openssl/aes.h>
#include <openssl/evp.h>

#include <string>
#include <vector>

int CCrypter::BytesToKeySHA512AES(const std::vector<unsigned char>& chSalt, const SecureString& strKeyData, int count, unsigned char *key,unsigned char *iv) const
{
    // This mimics the behavior of openssl's EVP_BytesToKey with an aes256cbc
    // cipher and sha512 message digest. Because sha512's output size (64b) is
    // greater than the aes256 block size (16b) + aes256 key size (32b),
    // there's no need to process more than once (D_0).

    if(!count || !key || !iv)
        return 0;

    unsigned char buf[CSHA512::OUTPUT_SIZE];
    CSHA512 di;

    di.Write((const unsigned char*)strKeyData.c_str(), strKeyData.size());
    di.Write(chSalt.data(), chSalt.size());
    di.Finalize(buf);

    for(int i = 0; i != count - 1; i++)
        di.Reset().Write(buf, sizeof(buf)).Finalize(buf);

    memcpy(key, buf, WALLET_CRYPTO_KEY_SIZE);
    memcpy(iv, buf + WALLET_CRYPTO_KEY_SIZE, WALLET_CRYPTO_IV_SIZE);
    memory_cleanse(buf, sizeof(buf));
    return WALLET_CRYPTO_KEY_SIZE;
}

bool CCrypter::SetKeyFromPassphrase(const SecureString& strKeyData, const std::vector<unsigned char>& chSalt, const unsigned int nRounds, const unsigned int nDerivationMethod)
{
    if (nRounds < 1 || chSalt.size() != WALLET_CRYPTO_SALT_SIZE)
        return false;

    int i = 0;
    if (nDerivationMethod == 0)
        i = BytesToKeySHA512AES(chSalt, strKeyData, nRounds, vchKey.data(), vchIV.data());

    if (i != (int)WALLET_CRYPTO_KEY_SIZE)
    {
        memory_cleanse(vchKey.data(), vchKey.size());
        memory_cleanse(vchIV.data(), vchIV.size());
        return false;
    }

    fKeySet = true;
    return true;
}

bool CCrypter::SetKey(const CKeyingMaterial& chNewKey, const std::vector<unsigned char>& chNewIV)
{
    if (chNewKey.size() != WALLET_CRYPTO_KEY_SIZE || chNewIV.size() != WALLET_CRYPTO_IV_SIZE)
        return false;

    memcpy(vchKey.data(), chNewKey.data(), chNewKey.size());
    memcpy(vchIV.data(), chNewIV.data(), chNewIV.size());

    fKeySet = true;
    return true;
}

bool CCrypter::Encrypt(const CKeyingMaterial& vchPlaintext, std::vector<unsigned char> &vchCiphertext) const
{
    if (!fKeySet)
        return false;

    // max ciphertext len for a n bytes of plaintext is
    // n + AES_BLOCKSIZE bytes
    vchCiphertext.resize(vchPlaintext.size() + AES_BLOCKSIZE);

    AES256CBCEncrypt enc(vchKey.data(), vchIV.data(), true);
    size_t nLen = enc.Encrypt(&vchPlaintext[0], vchPlaintext.size(), vchCiphertext.data());
    if(nLen < vchPlaintext.size())
        return false;
    vchCiphertext.resize(nLen);

    return true;
}

bool CCrypter::Decrypt(const std::vector<unsigned char>& vchCiphertext, CKeyingMaterial& vchPlaintext) const
{
    if (!fKeySet)
        return false;

    // plaintext will always be equal to or lesser than length of ciphertext
    int nLen = vchCiphertext.size();

    vchPlaintext.resize(nLen);

    AES256CBCDecrypt dec(vchKey.data(), vchIV.data(), true);
    nLen = dec.Decrypt(vchCiphertext.data(), vchCiphertext.size(), &vchPlaintext[0]);
    if(nLen == 0)
        return false;
    vchPlaintext.resize(nLen);
    return true;
}


static bool EncryptSecret(const CKeyingMaterial& vMasterKey, const CKeyingMaterial &vchPlaintext, const uint256& nIV, std::vector<unsigned char> &vchCiphertext)
{
    CCrypter cKeyCrypter;
    std::vector<unsigned char> chIV(WALLET_CRYPTO_IV_SIZE);
    memcpy(chIV.data(), &nIV, WALLET_CRYPTO_IV_SIZE);
    if(!cKeyCrypter.SetKey(vMasterKey, chIV))
        return false;
    return cKeyCrypter.Encrypt(*((const CKeyingMaterial*)&vchPlaintext), vchCiphertext);
}

// Dash
// General secure AES 256 CBC encryption routine
bool EncryptAES256(const SecureString& sKey, const SecureString& sPlaintext, const std::string& sIV, std::string& sCiphertext)
{
    // max ciphertext len for a n bytes of plaintext is
    // n + AES_BLOCK_SIZE - 1 bytes
    int nLen = sPlaintext.size();
    int nCLen = nLen + AES_BLOCK_SIZE;
    int nFLen = 0;

    // Verify key sizes
    if(sKey.size() != 32 || sIV.size() != AES_BLOCK_SIZE) {
        LogPrintf("crypter EncryptAES256 - Invalid key or block size: Key: %d sIV:%d\n", sKey.size(), sIV.size());
        return false;
    }

    // Prepare output buffer
    sCiphertext.resize(nCLen);

    // Perform the encryption
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    if (!ctx) return false;

    bool fOk = true;

    EVP_CIPHER_CTX_init(ctx);
    if (fOk) fOk = EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*) &sKey[0], (const unsigned char*) &sIV[0]);
    if (fOk) fOk = EVP_EncryptUpdate(ctx, (unsigned char*) &sCiphertext[0], &nCLen, (const unsigned char*) &sPlaintext[0], nLen);
    if (fOk) fOk = EVP_EncryptFinal_ex(ctx, (unsigned char*) (&sCiphertext[0])+nCLen, &nFLen);
    EVP_CIPHER_CTX_cleanup(ctx);

    EVP_CIPHER_CTX_free(ctx);

    if (!fOk) return false;

    sCiphertext.resize(nCLen + nFLen);
    return true;
}
//

static bool DecryptSecret(const CKeyingMaterial& vMasterKey, const std::vector<unsigned char>& vchCiphertext, const uint256& nIV, CKeyingMaterial& vchPlaintext)
{
    CCrypter cKeyCrypter;
    std::vector<unsigned char> chIV(WALLET_CRYPTO_IV_SIZE);
    memcpy(chIV.data(), &nIV, WALLET_CRYPTO_IV_SIZE);
    if(!cKeyCrypter.SetKey(vMasterKey, chIV))
        return false;
    return cKeyCrypter.Decrypt(vchCiphertext, *((CKeyingMaterial*)&vchPlaintext));
}

// Dash
bool DecryptAES256(const SecureString& sKey, const std::string& sCiphertext, const std::string& sIV, SecureString& sPlaintext)
{
    // plaintext will always be equal to or lesser than length of ciphertext
    int nLen = sCiphertext.size();
    int nPLen = nLen, nFLen = 0;

    // Verify key sizes
    if(sKey.size() != 32 || sIV.size() != AES_BLOCK_SIZE) {
        LogPrintf("crypter DecryptAES256 - Invalid key or block size\n");
        return false;
    }

    sPlaintext.resize(nPLen);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    if (!ctx) return false;

    bool fOk = true;

    EVP_CIPHER_CTX_init(ctx);
    if (fOk) fOk = EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*) &sKey[0], (const unsigned char*) &sIV[0]);
    if (fOk) fOk = EVP_DecryptUpdate(ctx, (unsigned char *) &sPlaintext[0], &nPLen, (const unsigned char *) &sCiphertext[0], nLen);
    if (fOk) fOk = EVP_DecryptFinal_ex(ctx, (unsigned char *) (&sPlaintext[0])+nPLen, &nFLen);
    EVP_CIPHER_CTX_cleanup(ctx);

    EVP_CIPHER_CTX_free(ctx);

    if (!fOk) return false;

    sPlaintext.resize(nPLen + nFLen);
    return true;
}
//

static bool DecryptKey(const CKeyingMaterial& vMasterKey, const std::vector<unsigned char>& vchCryptedSecret, const CPubKey& vchPubKey, CKey& key)
{
    CKeyingMaterial vchSecret;
    if(!DecryptSecret(vMasterKey, vchCryptedSecret, vchPubKey.GetHash(), vchSecret))
        return false;

    if (vchSecret.size() != 32)
        return false;

    key.Set(vchSecret.begin(), vchSecret.end(), vchPubKey.IsCompressed());
    return key.VerifyPubKey(vchPubKey);
}

bool CCryptoKeyStore::SetCrypted()
{
    LOCK(cs_KeyStore);
    if (fUseCrypto)
        return true;
    if (!mapKeys.empty())
        return false;
    fUseCrypto = true;
    return true;
}

// Dash
// This function should be used in a different combinations to determine
// if CCryptoKeyStore is fully locked so that no operations requiring access
// to private keys are possible:
//      IsLocked(true)
// or if CCryptoKeyStore's private keys are available for mixing only:
//      !IsLocked(true) && IsLocked()
// or if they are available for everything:
//      !IsLocked()
//bool CCryptoKeyStore::IsLocked() const
bool CCryptoKeyStore::IsLocked(bool fForMixing) const
//
{
    if (!IsCrypted()) {
        return false;
    }
    // Dash
    //LOCK(cs_KeyStore);
    //return vMasterKey.empty();
    bool result;
    {
        LOCK(cs_KeyStore);
        result = vMasterKey.empty();
    }

    // fForMixing   fOnlyMixingAllowed  return
    // ---------------------------------------
    // true         true                result
    // true         false               result
    // false        true                true
    // false        false               result

    if(!fForMixing && fOnlyMixingAllowed) return true;

    return result;
    //
}

// Dash
//bool CCryptoKeyStore::Lock()
bool CCryptoKeyStore::Lock(bool fAllowMixing)
//
{
    if (!SetCrypted())
        return false;

    // Dash
    //{
    if (!fAllowMixing) {
    //
        LOCK(cs_KeyStore);
        vMasterKey.clear();
    }

    // Dash
    fOnlyMixingAllowed = fAllowMixing;
    //
    NotifyStatusChanged(this);
    return true;
}

// Dash
//bool CCryptoKeyStore::Unlock(const CKeyingMaterial& vMasterKeyIn, bool accept_no_keys)
bool CCryptoKeyStore::Unlock(const CKeyingMaterial& vMasterKeyIn, bool accept_no_keys, bool fForMixingOnly)
//
{
    {
        LOCK(cs_KeyStore);
        if (!SetCrypted())
            return false;

        bool keyPass = mapCryptedKeys.empty(); // Always pass when there are no encrypted keys
        bool keyFail = false;
        CryptedKeyMap::const_iterator mi = mapCryptedKeys.begin();
        for (; mi != mapCryptedKeys.end(); ++mi)
        {
            const CPubKey &vchPubKey = (*mi).second.first;
            const std::vector<unsigned char> &vchCryptedSecret = (*mi).second.second;
            CKey key;
            if (!DecryptKey(vMasterKeyIn, vchCryptedSecret, vchPubKey, key))
            {
                keyFail = true;
                break;
            }
            keyPass = true;
            if (fDecryptionThoroughlyChecked)
                break;
        }
        if (keyPass && keyFail)
        {
            LogPrintf("The wallet is probably corrupted: Some keys decrypt but not all.\n");
            throw std::runtime_error("Error unlocking wallet: some keys decrypt but not all. Your wallet file may be corrupt.");
        }
        if (keyFail || (!keyPass && !accept_no_keys))
            return false;
        vMasterKey = vMasterKeyIn;
        fDecryptionThoroughlyChecked = true;
    }
    // Dash
    fOnlyMixingAllowed = fForMixingOnly;
    //
    NotifyStatusChanged(this);
    return true;
}

bool CCryptoKeyStore::AddKeyPubKey(const CKey& key, const CPubKey &pubkey)
{
    LOCK(cs_KeyStore);
    if (!IsCrypted()) {
        return CBasicKeyStore::AddKeyPubKey(key, pubkey);
    }

    if (IsLocked()) {
        return false;
    }

    std::vector<unsigned char> vchCryptedSecret;
    CKeyingMaterial vchSecret(key.begin(), key.end());
    if (!EncryptSecret(vMasterKey, vchSecret, pubkey.GetHash(), vchCryptedSecret)) {
        return false;
    }

    if (!AddCryptedKey(pubkey, vchCryptedSecret)) {
        return false;
    }
    return true;
}


bool CCryptoKeyStore::AddCryptedKey(const CPubKey &vchPubKey, const std::vector<unsigned char> &vchCryptedSecret)
{
    LOCK(cs_KeyStore);
    if (!SetCrypted()) {
        return false;
    }

    mapCryptedKeys[vchPubKey.GetID()] = make_pair(vchPubKey, vchCryptedSecret);
    ImplicitlyLearnRelatedKeyScripts(vchPubKey);
    return true;
}

bool CCryptoKeyStore::HaveKey(const CKeyID &address) const
{
    LOCK(cs_KeyStore);
    if (!IsCrypted()) {
        return CBasicKeyStore::HaveKey(address);
    }
    return mapCryptedKeys.count(address) > 0;
}

bool CCryptoKeyStore::GetKey(const CKeyID &address, CKey& keyOut) const
{
    LOCK(cs_KeyStore);
    if (!IsCrypted()) {
        return CBasicKeyStore::GetKey(address, keyOut);
    }

    CryptedKeyMap::const_iterator mi = mapCryptedKeys.find(address);
    if (mi != mapCryptedKeys.end())
    {
        const CPubKey &vchPubKey = (*mi).second.first;
        const std::vector<unsigned char> &vchCryptedSecret = (*mi).second.second;
        return DecryptKey(vMasterKey, vchCryptedSecret, vchPubKey, keyOut);
    }
    return false;
}

bool CCryptoKeyStore::GetPubKey(const CKeyID &address, CPubKey& vchPubKeyOut) const
{
    LOCK(cs_KeyStore);
    if (!IsCrypted())
        return CBasicKeyStore::GetPubKey(address, vchPubKeyOut);

    CryptedKeyMap::const_iterator mi = mapCryptedKeys.find(address);
    if (mi != mapCryptedKeys.end())
    {
        vchPubKeyOut = (*mi).second.first;
        return true;
    }
    // Check for watch-only pubkeys
    return CBasicKeyStore::GetPubKey(address, vchPubKeyOut);
}

std::set<CKeyID> CCryptoKeyStore::GetKeys() const
{
    LOCK(cs_KeyStore);
    if (!IsCrypted()) {
        return CBasicKeyStore::GetKeys();
    }
    std::set<CKeyID> set_address;
    for (const auto& mi : mapCryptedKeys) {
        set_address.insert(mi.first);
    }
    return set_address;
}

bool CCryptoKeyStore::EncryptKeys(CKeyingMaterial& vMasterKeyIn)
{
    LOCK(cs_KeyStore);
    if (!mapCryptedKeys.empty() || IsCrypted())
        return false;

    fUseCrypto = true;
    for (const KeyMap::value_type& mKey : mapKeys)
    {
        const CKey &key = mKey.second;
        CPubKey vchPubKey = key.GetPubKey();
        CKeyingMaterial vchSecret(key.begin(), key.end());
        std::vector<unsigned char> vchCryptedSecret;
        if (!EncryptSecret(vMasterKeyIn, vchSecret, vchPubKey.GetHash(), vchCryptedSecret))
            return false;
        if (!AddCryptedKey(vchPubKey, vchCryptedSecret))
            return false;
    }
    mapKeys.clear();
    return true;
}
