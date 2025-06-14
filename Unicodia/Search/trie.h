#pragma once

// STL
#include <unordered_map>
#include <memory>

// Libs
#include "u_Vector.h"
#include "u_DumbSp.h"

// Search
#include "defs.h"

namespace srh {

    ///  @tparam  R result
    ///     Just to get rid of concrete result
    ///
    struct TrieNode : public dumb::SpTarget {
    public:
        constexpr TrieNode(unsigned short anZwjs)
            : fnZwjs(anZwjs) {}

        const uc::LibNode* result() const noexcept { return fResult; }
        bool isFinal() const noexcept { return fIsFinal; }
        unsigned short nZwjs() const noexcept { return fnZwjs; }

        /// @return   never null
        TrieNode* add(char32_t c);

        inline void setFinal(const uc::LibNode* res)
        {
            fIsFinal = true;
            fResult = res;
        }

        inline const TrieNode* unsafeFind(char32_t c) const;
        const TrieNode* find(char32_t c) const;
    protected:
        struct Link {
            dumb::Sp<TrieNode> target;
        };
        using M = std::unordered_map<char32_t, Link>;

        const uc::LibNode* fResult {};
        std::unique_ptr<M> children;
        const unsigned short fnZwjs;
        bool fIsFinal = false;

        constexpr TrieNode() : fnZwjs(0) {}
    };

    struct TrieRoot : public TrieNode {
    private:
        using Super = TrieNode;
        using Node = TrieNode;
        using typename Super::M;
        using Super::children;
    public:
        constexpr TrieRoot() : Super() {}
        /// @return [+] has some data
        operator bool() const noexcept { return children.get(); }
        void add(std::u32string_view s, const uc::LibNode* res);
        template <class... Args>
            void addMulti(const uc::LibNode* res, Args&&... args)
            {
                char32_t v[] { std::forward<Args>(args)... };
                std::u32string_view sv { v, std::size(v) };
                add(sv, res);
            }
        SafeVector<DecodedLine> decode(std::u32string_view s) const;
    };

}   // namespace srh

inline const srh::TrieNode* srh::TrieNode::unsafeFind(char32_t c) const
{
    if (children) {
        if (auto x = children->find(c); x != children->end()) {
            return x->second.target.get();
        }
    }
    return nullptr;
}
