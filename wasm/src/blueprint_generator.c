#include <assert.h>
#include <zlib.h>
#include "blueprint_generator.h"
#include "factorio_ui.h"

static const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const string DISPLAY_PANEL = str("display-panel");

blueprint Generate_Image_Blueprint(Factorio_Icon_Frame icon_frame, arena* a) {
    blueprint new_blueprint = blueprint_new();
    int entity_idx = 0;
    for (int y = 0; y < icon_frame.height; ++y) {
        for (int x = 0; x < icon_frame.width; ++x) {
            Factorio_Icon *icon = Get_Icon(icon_frame.icon_ids[y * icon_frame.width + x]);
            if (icon) {
                entity_icon *new_entity_icon = entity_icon_new(&icon->name, a);
                entity *new_entity = entity_new(entity_idx++, (string *)&DISPLAY_PANEL, x, y, new_entity_icon, a);
                blueprint_add_entity(&new_blueprint, new_entity, a);
            }
        }
    }
    return new_blueprint;
}

blueprint Generate_Video_Blueprint(Factorio_Icon_Video icon_video, arena *a) {
    blueprint new_blueprint = blueprint_new();
    // TODO
    return new_blueprint;
}

ptrdiff_t Base64_Encode(string src, string_buffer* dest) {
    unsigned char *out, *pos;
    const unsigned char *end, *in;

    ptrdiff_t olen;

    olen = 4*((src.len + 2) / 3); /* 3-byte blocks to 4-byte */

    if (olen < src.len || olen + dest->str.len > dest->cap) {
        return 0; // integer overflow or output is too large
    }

    out = dest->str.data + dest->str.len;
    end = src.data + src.len;
    in = src.data;
    pos = out;

    while (end - in >= 3) {
        *pos++ = base64_table[in[0] >> 2];
        *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = base64_table[in[2] & 0x3f];
        in += 3;
    }

    if (end - in) {
        *pos++ = base64_table[in[0] >> 2];
        if (end - in == 1) {
            *pos++ = base64_table[(in[0] & 0x03) << 4];
            *pos++ = '=';
        }
        else {
            *pos++ = base64_table[((in[0] & 0x03) << 4) |
                (in[1] >> 4)];
            *pos++ = base64_table[(in[1] & 0x0f) << 2];
        }
        *pos++ = '=';
    }

    dest->str.len += olen;
    return olen;
}

void Encode_Blueprint(blueprint blueprint, string_buffer *out, arena scratch) {
    int ret;
    int flush = Z_NO_FLUSH;
    ptrdiff_t have;
    z_stream strm;

    // Blueprint string is prepended with a 0
    string_buffer_add_str(out, string("0"));

    // Used to generate the raw blueprint string in chunks
    blueprint_stream bp_stream = blueprint_stream_new(blueprint, Z_CHUNK, &scratch);
    // Used to store the compressed blueprint string in chunks
    string_buffer bp_compressed = string_buffer_new(Z_CHUNK, &scratch);

    // Initialize the zlib stream for compression
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_BEST_COMPRESSION);
    if (ret != Z_OK) return;

    // Compress until end of blueprint
    do {
        // Add next chunk of text to the input
        string in_str = blueprint_stream_next(&bp_stream);
        strm.avail_in = in_str.len;
        strm.next_in = in_str.data;
        if (in_str.len < Z_CHUNK) {
            flush = Z_FINISH;
        }
        // Compress until the input is empty
        do {
            strm.avail_out = string_buffer_remaining(&bp_compressed);
            strm.next_out = bp_compressed.str.data + bp_compressed.str.len;
            have = strm.avail_out;
            ret = deflate(&strm, flush);
            assert(ret != Z_STREAM_ERROR);
            // Update output buffer length
            bp_compressed.str.len += have - strm.avail_out;
            if (strm.avail_out == 0) {
                // Save compressed chunk in base64 and clear the buffer
                Base64_Encode(bp_compressed.str, out);
                string_buffer_clear(&bp_compressed);
            }
        } while (strm.avail_in > 0 || (flush == Z_FINISH && ret != Z_STREAM_END));
    } while (flush != Z_FINISH);

    // Save remaining compressed data in base64
    if (bp_compressed.str.len > 0) {
        Base64_Encode(bp_compressed.str, out);
    }

    // Clean up
    deflateEnd(&strm);
}