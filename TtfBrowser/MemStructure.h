#pragma once

#include "Fonts/MemFont.h"


namespace mfs {

    enum class Type { FONT, BLOCK, CMAP };

    struct Text {
        std::string value;
        bool isConstantLength;
    };

    class Obj { // interface
    public:
        virtual Type type() const noexcept = 0;
        virtual size_t nChildren() const noexcept = 0;
        virtual const Obj& childAt(size_t i) const = 0;
        virtual Text text() const noexcept = 0;

        virtual bool hasData() const noexcept = 0;
        /// @warning   body, no headers
        virtual size_t bodyOffset() const noexcept = 0;
        virtual size_t bodySize() const noexcept = 0;

        // Need virtual dtor
        virtual ~Obj() = default;

        // Utils
        Buf1d<char> body(Buf1d<char> entireFile) const
            { return entireFile.sliceMid(bodyOffset(), bodySize()); }
        Buf1d<const char> body(Buf1d<const char> entireFile) const
            { return entireFile.sliceMid(bodyOffset(), bodySize()); }
    };

    class Block : public Obj
    {
    public:
        const mf::Block& slave;

        Block(const mf::Block& aSlave) : slave(aSlave) {}

        // Obj
        Type type() const noexcept override { return Type::BLOCK; }
        size_t nChildren() const noexcept override { return 0; }
        const Obj& childAt(size_t i) const override;
        Text text() const noexcept override
            { return { std::string { slave.name.toSv() }, true }; };
        bool hasData() const noexcept override { return true; }
        size_t bodyOffset() const noexcept override { return slave.posInFile; }
        size_t bodySize() const noexcept override { return slave.length; }
    };

    template <class Child>
    class BlockEx : public Block
    {
    public:
        size_t nChildren() const noexcept override { return 0; }
        const Obj& childAt(size_t i) const override;
    private:
        SafeVector<std::shared_ptr<Child>> fChildren;
    };

    class Cmap : public Obj
    {

    };

    class CmapBlock : public BlockEx<Cmap>
    {

    };

    class Font final : public Obj
    {
    public:
        void clear();
        void loadFrom(const MemFont& font);

        // Obj
        Type type() const noexcept override { return Type::FONT; }
        size_t nChildren() const noexcept override;
        const Block& childAt(size_t i) const override;
        Text text() const noexcept override { return { "Font", false }; }
        bool hasData() const noexcept override { return true; }
        size_t bodyOffset() const noexcept override { return 0; }
        size_t bodySize() const noexcept override { return fileSize; }
    private:
        const MemFont* slave = nullptr;
        size_t fileSize = 0;
        std::shared_ptr<CmapBlock> cmapBlock;
        SafeVector<std::shared_ptr<Block>> blocks;
    };

}   // namespace mf
