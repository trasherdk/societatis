// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2020, The Qwertycoin Group.
// Copyright (c) 2020, Societatis.io
//
// This file is part of Societatis.
//
// Societatis is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Societatis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Societatis.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <crypto/crypto.h>
#include <CryptoNoteCore/CryptoNoteBasic.h>
#include <CryptoNoteCore/CryptoNoteSerialization.h>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/ITransaction.h>
#include <Logging/LoggerRef.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <ITransfersContainer.h>

namespace CryptoNote {

struct TransactionOutputInformationIn;

class SpentOutputDescriptor
{
public:
    SpentOutputDescriptor();
    explicit SpentOutputDescriptor(const TransactionOutputInformationIn &transactionInfo);
    explicit SpentOutputDescriptor(const Crypto::KeyImage *keyImage);
    SpentOutputDescriptor(uint64_t amount, uint32_t globalOutputIndex);

    void assign(const Crypto::KeyImage *keyImage);
    void assign(uint64_t amount, uint32_t globalOutputIndex);

    bool isValid() const;

    size_t hash() const;

    bool operator==(const SpentOutputDescriptor &other) const;

private:
    TransactionTypes::OutputType m_type;
    union {
        const Crypto::KeyImage *m_keyImage;
        struct
        {
            uint64_t m_amount;
            uint32_t m_globalOutputIndex;
        };
    };
};

struct SpentOutputDescriptorHasher
{
    size_t operator()(const SpentOutputDescriptor &descriptor) const
    {
        return descriptor.hash();
    }
};

struct TransactionOutputInformationIn : public TransactionOutputInformation
{
    Crypto::KeyImage keyImage; // WARNING: Used only for TransactionTypes::OutputType::Key!
};

struct TransactionOutputInformationEx : public TransactionOutputInformationIn
{
    SpentOutputDescriptor getSpentOutputDescriptor() const { return SpentOutputDescriptor(*this); }
    const Crypto::Hash& getTransactionHash() const { return transactionHash; }

    void serialize(CryptoNote::ISerializer &s)
    {
        s(reinterpret_cast<uint8_t &>(type), "type");
        s(amount, "");
        serializeGlobalOutputIndex(s, globalOutputIndex, "");
        s(outputInTransaction, "");
        s(transactionPublicKey, "");
        s(keyImage, "");
        s(unlockTime, "");
        serializeBlockHeight(s, blockHeight, "");
        s(transactionIndex, "");
        s(transactionHash, "");
        s(visible, "");

        if (type == TransactionTypes::OutputType::Key) {
            s(outputKey, "");
        } else if (type == TransactionTypes::OutputType::Multisignature) {
            s(requiredSignatures, "");
        }
    }

    uint64_t unlockTime;
    uint32_t blockHeight;
    uint32_t transactionIndex;
    bool visible;

};

struct TransactionBlockInfo
{
    void serialize(ISerializer &s)
    {
        serializeBlockHeight(s, height, "height");
        s(timestamp, "timestamp");
        s(transactionIndex, "transactionIndex");
    }

    uint32_t height;
    uint64_t timestamp;
    uint32_t transactionIndex;
};

struct SpentTransactionOutput : TransactionOutputInformationEx
{
    const Crypto::Hash &getSpendingTransactionHash() const
    {
        return spendingTransactionHash;
    }

    void serialize(ISerializer &s)
    {
        TransactionOutputInformationEx::serialize(s);
        s(spendingBlock, "spendingBlock");
        s(spendingTransactionHash, "spendingTransactionHash");
        s(inputInTransaction, "inputInTransaction");
    }

    TransactionBlockInfo spendingBlock;
    Crypto::Hash spendingTransactionHash;
    uint32_t inputInTransaction;
};

class TransfersContainer : public ITransfersContainer
{
public:
    TransfersContainer(const CryptoNote::Currency &currency,
                       Logging::ILogger &logger,
                       size_t transactionSpendableAge,
                       size_t safeTransactionSpendableAge);

    bool addTransaction(const TransactionBlockInfo &block,
                        const ITransactionReader &tx,
                        const std::vector<TransactionOutputInformationIn> &transfers);
    bool deleteUnconfirmedTransaction(const Crypto::Hash& transactionHash);
    bool markTransactionConfirmed(const TransactionBlockInfo &block,
                                  const Crypto::Hash &transactionHash,
                                  const std::vector<uint32_t> &globalIndices);

    std::vector<Crypto::Hash> detach(uint32_t height);
    bool advanceHeight(uint32_t height);

    // ITransfersContainer
    size_t transfersCount() const override;
    size_t transactionsCount() const override;
    uint64_t balance(uint32_t flags) const override;
    void getOutputs(
        std::vector<TransactionOutputInformation> &transfers,
        uint32_t flags) const override;
    bool getTransactionInformation(
        const Crypto::Hash &transactionHash,
        TransactionInformation &info,
        uint64_t *amountIn = nullptr,
        uint64_t *amountOut = nullptr) const override;
    std::vector<TransactionOutputInformation> getTransactionOutputs(
        const Crypto::Hash &transactionHash,
        uint32_t flags) const override;
    // only type flags are feasible for this function
    std::vector<TransactionOutputInformation> getTransactionInputs(
        const Crypto::Hash &transactionHash,
        uint32_t flags) const override;
    void getUnconfirmedTransactions(std::vector<Crypto::Hash> &transactions) const override;
    std::vector<TransactionSpentOutputInformation> getSpentOutputs() const override;
    void markTransactionSafe(const Crypto::Hash &transactionHash) override;
    void getSafeTransactions(std::vector<Crypto::Hash> &transactions) const override;

    // IStreamSerializable
    void save(std::ostream &os) override;
    void load(std::istream &in) override;

private:
    struct ContainingTransactionIndex { };
    struct SpendingTransactionIndex { };
    struct SpentOutputDescriptorIndex { };

    typedef boost::multi_index_container<
        TransactionInformation,
        boost::multi_index::indexed_by<
            boost::multi_index::hashed_unique<
                BOOST_MULTI_INDEX_MEMBER(TransactionInformation, Crypto::Hash, transactionHash)
            >,
            boost::multi_index::ordered_non_unique<
                BOOST_MULTI_INDEX_MEMBER(TransactionInformation, uint32_t, blockHeight)
            >
        >
    > TransactionMultiIndex;

    typedef boost::multi_index_container<
        TransactionOutputInformationEx,
        boost::multi_index::indexed_by<
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<SpentOutputDescriptorIndex>,
                boost::multi_index::const_mem_fun<
                    TransactionOutputInformationEx,
                    SpentOutputDescriptor,
                    &TransactionOutputInformationEx::getSpentOutputDescriptor
                >,
                SpentOutputDescriptorHasher
            >,
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<ContainingTransactionIndex>,
                boost::multi_index::const_mem_fun<
                    TransactionOutputInformationEx,
                    const Crypto::Hash &,
                    &TransactionOutputInformationEx::getTransactionHash
                >
            >
        >
    > UnconfirmedTransfersMultiIndex;

    typedef boost::multi_index_container<
        TransactionOutputInformationEx,
        boost::multi_index::indexed_by<
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<SpentOutputDescriptorIndex>,
                boost::multi_index::const_mem_fun<
                    TransactionOutputInformationEx,
                    SpentOutputDescriptor,
                    &TransactionOutputInformationEx::getSpentOutputDescriptor
                >,
                SpentOutputDescriptorHasher
            >,
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<ContainingTransactionIndex>,
                boost::multi_index::const_mem_fun<
                    TransactionOutputInformationEx,
                    const Crypto::Hash &,
                    &TransactionOutputInformationEx::getTransactionHash
                >
            >
        >
    > AvailableTransfersMultiIndex;

    typedef boost::multi_index_container<
        SpentTransactionOutput,
        boost::multi_index::indexed_by<
            boost::multi_index::hashed_unique<
                boost::multi_index::tag<SpentOutputDescriptorIndex>,
                boost::multi_index::const_mem_fun<
                    TransactionOutputInformationEx,
                    SpentOutputDescriptor,
                    &TransactionOutputInformationEx::getSpentOutputDescriptor
                >,
                SpentOutputDescriptorHasher
            >,
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<ContainingTransactionIndex>,
                boost::multi_index::const_mem_fun<
                    TransactionOutputInformationEx,
                    const Crypto::Hash &,
                    &SpentTransactionOutput::getTransactionHash
                >
            >,
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<SpendingTransactionIndex>,
                boost::multi_index::const_mem_fun <
                    SpentTransactionOutput,
                    const Crypto::Hash &,
                    &SpentTransactionOutput::getSpendingTransactionHash
                >
            >
        >
    > SpentTransfersMultiIndex;

private:
    void addTransaction(const TransactionBlockInfo &block, const ITransactionReader &tx);
    bool addTransactionOutputs(const TransactionBlockInfo &block,
                               const ITransactionReader &tx,
                               const std::vector<TransactionOutputInformationIn> &transfers);
    bool addTransactionInputs(const TransactionBlockInfo &block, const ITransactionReader &tx);
    void deleteTransactionTransfers(const Crypto::Hash &transactionHash);
    bool isSpendTimeUnlocked(uint64_t unlockTime) const;
    bool isIncluded(const TransactionOutputInformationEx &info, uint32_t flags) const;
    static bool isIncluded(TransactionTypes::OutputType type, uint32_t state, uint32_t flags);
    void updateTransfersVisibility(const Crypto::KeyImage &keyImage);

    void copyToSpent(const TransactionBlockInfo &block,
                     const ITransactionReader &tx,
                     size_t inputIndex,
                     const TransactionOutputInformationEx &output);
    void repair();

private:
    TransactionMultiIndex m_transactions;
    UnconfirmedTransfersMultiIndex m_unconfirmedTransfers;
    AvailableTransfersMultiIndex m_availableTransfers;
    SpentTransfersMultiIndex m_spentTransfers;

    mutable std::set<Crypto::Hash, Crypto::HashCompare> m_safeTxes;

    uint32_t m_currentHeight; // current height is needed to check if a transfer is unlocked
    size_t m_transactionSpendableAge;
    size_t m_safeTransactionSpendableAge;
    const CryptoNote::Currency &m_currency;
    mutable std::mutex m_mutex;
    Logging::LoggerRef m_logger;
};

} // namespace CryptoNote
