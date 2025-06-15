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
        TRANSIENT, FINAL, UNKNOWN_FLAG
    };

    ///  @tparam  R result
    ///     Just for unit-testing, surely small
    ///       (I don’t want to pull the entire Unicode base)
    ///     So you may add special logic here (e.g. full/partial qualificatio),
    ///       if you UT it and create a toy base that repeats the phenomenon
    ///       in smaller scale.
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
        TrieNode* add(char32_t c);

        inline void setFinal(R res)
        {
            fType = NodeType::FINAL;
            fResult = res;
        }

        inline const TrieNode* unsafeFind(char32_t c) const;
        const TrieNode* find(char32_t c) const;
    protected:
        struct Link {
            dumb::Sp<TrieNode> target;
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
    };

}   // namespace srh

template <class R>
inline const srh::TrieNode<R>* srh::TrieNode<R>::unsafeFind(char32_t c) const
{
    if (children) {
        if (auto x = children->find(c); x != children->end()) {
            return x->second.target.get();
        }
    }
    return nullptr;
}

template <class R>
const srh::TrieNode<R>* srh::TrieNode<R>::find(char32_t c) const
{
    if (!children)
        return nullptr;
    return unsafeFind(c);
}

template <class R>
srh::TrieNode<R>* srh::TrieNode<R>::add(char32_t c)
{
    if (!children)
        children = std::make_unique<M>();
    auto [it, _] = children->try_emplace(c);
    if (!it->second.target) {
        auto nNewZwjs = fnZwjs + static_cast<unsigned short>(c == cp::ZWJ);
        it->second.target = dumb::makeSp<TrieNode>(nNewZwjs);
    }
    return it->second.target.get();
}

template <class R>
void srh::TrieRoot<R>::add(std::u32string_view s, R res)
{
    Node* p = this;
    for (auto c : s) {
        p = p->add(c);
    }
    p->setFinal(std::move(res));
}

template <class R>
SafeVector<srh::Decoded<R>> srh::TrieRoot<R>::decode(std::u32string_view s) const
{
    struct Last {
        const Node* node = nullptr;
        size_t resetTime = 0;
        unsigned length = 0;
        EmojiType type = EmojiType::FULL;
    } lastKnown;

    SafeVector<srh::Decoded<R>> r;

    size_t index = 0;

    auto registerResult = [&]() {
        // Why +1? We do not search for single-char emoji, but if…
        //   iLastPos == 0, length == 1 → how to make 0 out of them?
        r.emplace_back(
            lastKnown.node->result(),
            lastKnown.resetTime,
            lastKnown.length,
            lastKnown.type);
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
                p = child;
                if (p->type() != NodeType::TRANSIENT) {
                    lastKnown.node = p;
                    lastKnown.length = index + 1 - lastKnown.resetTime;
                    /// @todo [urgent] emoji type
                    lastKnown.type = (p->type() == NodeType::UNKNOWN_FLAG)
                                     ? EmojiType::UNKNOWN_FLAG : EmojiType::FULL;
                }
            } else if (p != this) {
                // p==&trieRoot → we already tried and no need 2nd time
                // We are at dead end!
                // Anyway move to root
                p = this;
                // Found smth? (never in root)
                if (lastKnown.node) {
                    registerResult();
                // Run through last character again, root’s children are always present
                } else if (auto child = p->unsafeFind(c)) {
                    // Why needed [A][A][D], all are flag components
                    lastKnown.resetTime = index;
                    p = child;
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
