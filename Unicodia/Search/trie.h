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

    template <class T>
    concept Result = std::is_default_constructible_v<T>;

    template <class T>
    concept Flag = std::is_enum_v<T>;

    ///
    ///  @tparam  R result
    ///     Just to get rid of concrete result
    ///
    template <Result R, Flag F>
    struct TrieNode : public dumb::SpTarget {
    public:
        constexpr TrieNode(unsigned short aDepth, unsigned short anZwjs)
            : fDepth(aDepth), fnZwjs(anZwjs) {}

        const R& result() const noexcept { return fResult; }
        bool isFinal() const noexcept { return fIsFinal; }
        unsigned short depth() const noexcept { return fDepth; }
        unsigned short nZwjs() const noexcept { return fnZwjs; }

        /// @return   never null
        TrieNode* add(char32_t c);

        inline void setFinal(R res)
        {
            fIsFinal = true;
            fResult = res;
        }

        inline const TrieNode* unsafeFind(char32_t c) const;
        const TrieNode* find(char32_t c) const;
    protected:
        struct Link {
            dumb::Sp<TrieNode<R, F>> target;
        };
        using M = std::unordered_map<char32_t, Link>;

        R fResult {};
        std::unique_ptr<M> children;
        const unsigned short fDepth;
        const unsigned short fnZwjs;
        bool fIsFinal = false;

        constexpr TrieNode() : fDepth(0), fnZwjs(0) {}
    };

    template <Result R, Flag F>
    struct TrieRoot : public TrieNode<R, F> {
    private:
        using Super = TrieNode<R, F>;
        using Node = TrieNode<R, F>;
        using typename Super::M;
        using Super::children;
    public:
        constexpr TrieRoot() : Super() {}
        /// @return [+] has some data
        operator bool() const noexcept { return children.get(); }
        void add(std::u32string_view s, const R& res);
        template <class... Args>
            void addMulti(const R& res, Args&&... args)
            {
                char32_t v[] { std::forward<Args>(args)... };
                std::u32string_view sv { v, std::size(v) };
                add(sv, res);
            }
        SafeVector<Decoded<R>> decode(std::u32string_view s) const;
    };


}   // namespace srh

template <srh::Result R, srh::Flag F>
inline auto srh::TrieNode<R, F>::unsafeFind(char32_t c) const
        -> const srh::TrieNode<R, F>*
{
    if (children) {
        if (auto x = children->find(c); x != children->end()) {
            return x->second.target.get();
        }
    }
    return nullptr;
}

template <srh::Result R, srh::Flag F>
auto srh::TrieNode<R, F>::find(char32_t c) const -> const srh::TrieNode<R, F>*
{
    if (!children)
        return nullptr;
    return unsafeFind(c);
}

template <srh::Result R, srh::Flag F>
auto srh::TrieNode<R, F>::add(char32_t c) -> srh::TrieNode<R, F>*
{
    if (!children)
        children = std::make_unique<M>();
    auto [it, _] = children->try_emplace(c);
    if (!it->second.target) {
        auto nNewZwjs = fnZwjs + static_cast<unsigned short>(c == cp::ZWJ);
        it->second.target = dumb::makeSp<TrieNode<R, F>>(fDepth + 1, nNewZwjs);
    }
    return it->second.target.get();
}

template <srh::Result R, srh::Flag F>
void srh::TrieRoot<R, F>::add(std::u32string_view s, const R& res)
{
    Node* p = this;
    for (auto c : s) {
        p = p->add(c);
    }
    p->setFinal(res);
}

template <srh::Result R, srh::Flag F>
auto srh::TrieRoot<R, F>::decode(std::u32string_view s) const -> SafeVector<srh::Decoded<R>>
{
    static constexpr size_t NO_RESULT = -1;
    struct Last {
        const Node* node = nullptr;
        size_t iLastPos = NO_RESULT;
    } lastKnown;

    SafeVector<srh::Decoded<R>> r;

    size_t index = 0;

    auto registerResult = [&]() {
        // Why +1? We do not search for single-char emoji, but if…
        //   iLastPos == 0, length == 1 → how to make 0 out of them?
        r.emplace_back(
            lastKnown.iLastPos + 1 - lastKnown.node->depth(),
            lastKnown.node->result());
        // I do not want to make true Aho-Corasick here, so back down
        // Need backing down, counter-example: incomplete multi-racial kiss + A
        //  WOMAN RACE1 ZWJ HEART VS16 ZWJ KISS_MARK ZWJ MAN (no race2) A
        // After A we have WOMAN RACE1, but still want to identify HEART VS16
        index = lastKnown.iLastPos;
        lastKnown.node = nullptr;
    };

    for (; ; ++index) {
        const Node* p = this;
        for (; index < s.length(); ++index) {
            char32_t c = s[index];
            if (auto child = p->find(c)) {
                p = child;
                if (p->isFinal()) {
                    lastKnown.node = p;
                    lastKnown.iLastPos = index;
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
                    p = child;
                    // 1st is never decodeable
                }
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
