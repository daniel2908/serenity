/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/OwnPtr.h>
#include <AK/RefCounted.h>
#include <AK/RefPtr.h>
#include <LibGfx/Bitmap.h>
#include <LibGfx/Size.h>

namespace Gfx {

class Bitmap;

static constexpr size_t maximum_width_for_decoded_images = 16384;
static constexpr size_t maximum_height_for_decoded_images = 16384;

struct ImageFrameDescriptor {
    RefPtr<Bitmap> image;
    int duration { 0 };
};

class ImageDecoderPlugin {
public:
    virtual ~ImageDecoderPlugin() { }

    virtual IntSize size() = 0;
    virtual RefPtr<Gfx::Bitmap> bitmap() = 0;

    virtual void set_volatile() = 0;
    [[nodiscard]] virtual bool set_nonvolatile(bool& was_purged) = 0;

    virtual bool sniff() = 0;

    virtual bool is_animated() = 0;
    virtual size_t loop_count() = 0;
    virtual size_t frame_count() = 0;
    virtual ImageFrameDescriptor frame(size_t i) = 0;

protected:
    ImageDecoderPlugin() { }
};

class ImageDecoder : public RefCounted<ImageDecoder> {
public:
    static NonnullRefPtr<ImageDecoder> create(const u8* data, size_t size) { return adopt_ref(*new ImageDecoder(data, size)); }
    static NonnullRefPtr<ImageDecoder> create(const ByteBuffer& data) { return adopt_ref(*new ImageDecoder(data.data(), data.size())); }
    ~ImageDecoder();

    bool is_valid() const { return m_plugin; }

    IntSize size() const { return m_plugin ? m_plugin->size() : IntSize(); }
    int width() const { return size().width(); }
    int height() const { return size().height(); }
    RefPtr<Gfx::Bitmap> bitmap() const;
    void set_volatile()
    {
        if (m_plugin)
            m_plugin->set_volatile();
    }
    [[nodiscard]] bool set_nonvolatile(bool& was_purged) { return m_plugin ? m_plugin->set_nonvolatile(was_purged) : false; }
    bool sniff() const { return m_plugin ? m_plugin->sniff() : false; }
    bool is_animated() const { return m_plugin ? m_plugin->is_animated() : false; }
    size_t loop_count() const { return m_plugin ? m_plugin->loop_count() : 0; }
    size_t frame_count() const { return m_plugin ? m_plugin->frame_count() : 0; }
    ImageFrameDescriptor frame(size_t i) const { return m_plugin ? m_plugin->frame(i) : ImageFrameDescriptor(); }

private:
    ImageDecoder(const u8*, size_t);

    mutable OwnPtr<ImageDecoderPlugin> m_plugin;
};

}
