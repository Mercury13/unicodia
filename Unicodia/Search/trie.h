#pragma once

// STL
#include <unordered_map>
#include <memory>

// Libs
#include "u_Vector.h"
#include "u_DumbSp.h"
#include "UcCp.h"

// Search
#include "defs.h"

namespace srh {

    enum class NodeType : unsigned char {
        SRH_LEVELS,
        TRANSIENT,
    };

    ///  @tparam  R result
    ///   • Just for unit-testing, I don’t want to pull the entire Unicode base
    ///   • Surely small, the “combat” object is a pointer to entry
    ///   • So you may add special logic here (full/partial qualification,
    ///      skintone CPs), if you UT it and create a toy base that repeats
    ///      the phenomenon in smaller scale.
    ///
    template <class R>
    struct TrieNode : public dumb::SpTarget {
    public:
        constexpr TrieNode(unsigned short anZwjs)
            : fnZwjs(anZwjs) {}

        R result() const noexcept { return fResult; }
        NodeType type() const noexcept { return fType; }
        unsigned short nZwjs() const noexcept { return fnZwjs; }

        /// @return   never null
        TrieNode* add(char32_t c, EmojiLevel level);

        void link(char32_t c, EmojiLevel level, TrieNode* target);

        inline void setFinal(R res, NodeType type)
        {
            fType = type;
            fResult = res;
        }

        struct FindResult {
            const TrieNode* node = nullptr;
            EmojiLevel level = EmojiLevel::FULL;

            constexpr operator bool() const { return node; }
            constexpr const TrieNode& operator * () const { return *node; }
            constexpr const TrieNode* operator ->() const { return  node; }
        };

        FindResult find(char32_t c) const;
    protected:
        struct Link {
            dumb::Sp<TrieNode> target;
            EmojiLevel level = EmojiLevel::FULL;
        };
        using M = std::unordered_map<char32_t, Link>;

        R fResult {};
        std::unique_ptr<M> children;
        const unsigned short fnZwjs;
        NodeType fType = NodeType::TRANSIENT;

        constexpr TrieNode() : fnZwjs(0) {}
    };

    template <class R>
    struct TrieRoot : public TrieNode<R> {
    private:
        using Super = TrieNode<R>;
        using Node = TrieNode<R>;
        using typename Super::M;
        using Super::children;
    public:
        constexpr TrieRoot() : Super() {}
        /// @return [+] has some data
        operator bool() const noexcept { return children.get(); }
        void add(std::u32string_view s, R res);
        template <class... Args>
            void addMulti(R res, Args&&... args)
            {
                char32_t v[] { std::forward<Args>(args)... };
                std::u32string_view sv { v, std::size(v) };
                add(sv, std::move(res));
            }
        SafeVector<Decoded<R>> decode(std::u32string_view s) const;
        void addUnknownFlags(R wantedR);
    };

}   // namespace srh

template <class R>
auto srh::TrieNode<R>::find(char32_t c) const -> FindResult
{
    if (children) {
        if (auto x = children->find(c); x != children->end()) {
            return {
                .node = x->second.target.get(),
                .level = x->second.level,
            };
        }
    }
    return {};
}

template <class R>
srh::TrieNode<R>* srh::TrieNode<R>::add(char32_t c, EmojiLevel level)
{
    if (!children)
        children = std::make_unique<M>();
    auto [it, _] = children->try_emplace(c);
    if (!it->second.target) {
        auto nNewZwjs = fnZwjs + static_cast<unsigned short>(c == cp::ZWJ);
        it->second.target = dumb::makeSp<TrieNode>(nNewZwjs);
        it->second.level = level;
    }
    return it->second.target.get();
}

template <class R>
void srh::TrieNode<R>::link(char32_t c, EmojiLevel level, TrieNode* target)
{
    if (!children)
        children = std::make_unique<M>();
    auto [it, _] = children->try_emplace(c);
    if (!it->second.target) {
        it->second.target.reset(target);
        it->second.level = level;
    }
}

template <class R>
void srh::TrieRoot<R>::add(std::u32string_view s, R res)
{
    Node* p = this;
    Node* prevP = nullptr;
    bool hasZwj = false;
    for (auto c : s) {
        auto* oldP = p;
        p = oldP->add(c, EmojiLevel::FULL);
        if (c == cp::VS16) {
            prevP = oldP;
        } else {
            if (c == cp::ZWJ) {
                hasZwj = true;
            }
            if (prevP) {
                prevP->link(c, EmojiLevel::PART, p);
            }
            prevP = nullptr;
        }
    }
    if (hasZwj && prevP) {
        prevP->setFinal(res, NodeType::PART);
    }
    p->setFinal(std::move(res), NodeType::FULL);
}


template <class R>
void srh::TrieRoot<R>::addUnknownFlags(R wantedR)
{
    auto badNode = dumb::makeSp<TrieNode<R>>(0);
    badNode->setFinal(wantedR, NodeType::UNKNOWN_FLAG);
    for (char32_t c1 = cp::FLAG_A; c1 <= cp::FLAG_Z; ++c1) {
        auto node1 = Super::add(c1, EmojiLevel::UNKNOWN_FLAG);
        for (char32_t c2 = cp::FLAG_A; c2 <= cp::FLAG_Z; ++c2) {
            node1->link(c2, EmojiLevel::UNKNOWN_FLAG, badNode.get());
        }
    }
}


template <class R>
SafeVector<srh::Decoded<R>> srh::TrieRoot<R>::decode(std::u32string_view s) const
{
    struct Last {
        const Node* node = nullptr;
        size_t resetTime = 0;
        unsigned length = 0;
        EmojiLevel level = EmojiLevel::FULL;
    } lastKnown;

    SafeVector<srh::Decoded<R>> r;

    size_t index = 0;
    static constexpr EmojiLevel STARTING_LEVEL = EmojiLevel::FULL;
    EmojiLevel level = STARTING_LEVEL;

    auto registerResult = [&]() {
        // Why +1? We do not search for single-char emoji, but if…
        //   iLastPos == 0, length == 1 → how to make 0 out of them?
        r.emplace_back(
            lastKnown.node->result(),
            lastKnown.resetTime,
            lastKnown.length,
            lastKnown.level);
        // I do not want to make true Aho-Corasick here, so back down
        // Need backing down, counter-example: incomplete multi-racial kiss + A
        //  WOMAN RACE1 ZWJ HEART VS16 ZWJ KISS_MARK ZWJ MAN (no race2) A
        // After A we have WOMAN RACE1, but still want to identify HEART VS16
        lastKnown.resetTime += lastKnown.length;
        index = lastKnown.resetTime - 1;  // We’ll increase that index, so −1
        lastKnown.node = nullptr;
    };

    for (; ; ++index) {
        const Node* p = this;
        for (; index < s.length(); ++index) {
            char32_t c = s[index];
            if (auto child = p->find(c)) {
                p = child.node;
                // Max = worse
                level = std::max(level, child.level);
                if (p->type() != NodeType::TRANSIENT) {
                    lastKnown.node = p;
                    lastKnown.length = index + 1 - lastKnown.resetTime;
                    lastKnown.level = std::max(
                                level, static_cast<srh::EmojiLevel>(p->type()));

                }
            } else if (p != this) {
                // p==&trieRoot → we already tried and no need 2nd time
                // We are at dead end!
                // Anyway move to root
                p = this;
                level = STARTING_LEVEL;
                // Found smth? (never in root)
                if (lastKnown.node) {
                    registerResult();
                // Run through last character again, root’s children are always present
                } else if (auto child = p->find(c)) {
                    // Why needed: [A][A][D], all are flag components
                    lastKnown.resetTime = index;
                    p = child.node;
                    level = child.level;
                    // 1st is never decodeable
                } else {
                    // Nothing found, just reset
                    // Why needed: [A]a[A][D]
                    //  a = character, [A] = flag component A
                    lastKnown.resetTime = index + 1;
                }
            } else {
                // Nothing found, reset once again
                lastKnown.resetTime = index + 1;
            }
        }
        // Went out of loop — what have?
        if (lastKnown.node) {
            registerResult();
            // Back down even here! (incomplete multi-racial kiss, but no A afterwards)
        } else {
            break;  // The only exit from loop
        }
    }
    return r;
}
