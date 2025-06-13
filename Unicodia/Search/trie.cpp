#include "trie.h"

const srh::TrieNode* srh::TrieNode::find(char32_t c) const
{
    if (!children)
        return nullptr;
    return unsafeFind(c);
}

srh::TrieNode* srh::TrieNode::add(char32_t c)
{
    if (!children)
        children = std::make_unique<M>();
    auto [it, _] = children->try_emplace(c);
    if (!it->second.target) {
        auto nNewZwjs = fnZwjs + static_cast<unsigned short>(c == cp::ZWJ);
        it->second.target = dumb::makeSp<TrieNode>(fDepth + 1, nNewZwjs);
    }
    return it->second.target.get();
}

void srh::TrieRoot::add(std::u32string_view s, const uc::LibNode* res)
{
    Node* p = this;
    for (auto c : s) {
        p = p->add(c);
    }
    p->setFinal(res);
}

auto srh::TrieRoot::decode(std::u32string_view s) const -> SafeVector<Decoded<const uc::LibNode*>>
{
    static constexpr size_t NO_RESULT = -1;
    struct Last {
        const Node* node = nullptr;
        size_t iLastPos = NO_RESULT;
    } lastKnown;

    SafeVector<srh::Decoded<const uc::LibNode*>> r;

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
