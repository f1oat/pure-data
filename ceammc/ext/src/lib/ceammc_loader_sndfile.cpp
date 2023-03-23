/*****************************************************************************
 * Copyright 2017 Serge Poltavsky. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#include "ceammc_loader_sndfile.h"
#include "ceammc_log.h"
#include "fmt/core.h"
#include "soxr.h"

#include <cmath>
#include <iostream>

namespace ceammc {

namespace sound {

    LibSndFile::LibSndFile()
        : SoundFile("")
    {
    }

    LibSndFile::LibSndFile(const std::string& fname)
        : SoundFile(fname)
        , handle_(fname, SFM_READ, 0, 0, 0)
    {
        if (handle_.rawHandle() == 0) {
            std::cerr << "[SNDFILE] error while opening \"" << fname << "\": " << sf_strerror(0) << "\n";
        }
    }

    size_t LibSndFile::sampleCount() const
    {
        return handle_.frames();
    }

    size_t LibSndFile::sampleRate() const
    {
        return handle_.samplerate();
    }

    size_t LibSndFile::channels() const
    {
        return handle_.channels();
    }

    bool LibSndFile::isOpened() const
    {
        return handle_.rawHandle() != NULL;
    }

    bool LibSndFile::close()
    {
        handle_ = SndfileHandle();
        return true;
    }

    void LibSndFile::setFilename(const std::string& file)
    {
        fname_ = file;
    }

    long LibSndFile::read(t_word* dest, size_t sz, size_t ch, long offset, size_t max_samples)
    {
        if (!handle_) {
            handle_ = SndfileHandle(fname_, SFM_READ, user_format_, user_channels_, user_samplerate_);
            if (handle_.rawHandle() == 0) {
                LIB_ERR << fmt::format("[SNDFILE] can't open file for reading: '{}'", fname_);
                return -1;
            }
        }

        if (ch >= channels()) {
            LIB_ERR << fmt::format("[SNDFILE] invalid channel number: {}", ch);
            return -1;
        }

        if (resampleRatio() != 1)
            return readResampled(dest, sz, ch, offset, max_samples);

        t_word* x = dest;
        constexpr sf_count_t FRAME_COUNT = 256;
        const int n = channels();
        const sf_count_t IN_BUF_SIZE = FRAME_COUNT * n;
        float frame_buf[IN_BUF_SIZE];

        // move to beginning
        if (handle_.seek(offset, SEEK_SET) == -1) {
            LIB_ERR << fmt::format("[SNDFILE] can't seek to pos: {}", offset);
            return -1;
        }

        // read frames
        sf_count_t frames_read_total = 0;
        const sf_count_t steps = sf_count_t(sz) / FRAME_COUNT;
        // read full buffers
        for (sf_count_t i = 0; i < steps; i++) {
            const auto frames_read = handle_.readf(frame_buf, FRAME_COUNT);
            if (frames_read == 0)
                break;

            // write channel data to destination
            for (sf_count_t j = 0; j < frames_read && frames_read_total < max_samples; j++) {
                x->w_float = frame_buf[j * n + ch] * gain();
                x++;
                frames_read_total++;
            }

            // seek to next
            if (handle_.seek(offset + frames_read_total, SEEK_SET) == -1)
                break;
        }

        // read remaining
        if (sf_count_t(sz) % FRAME_COUNT != 0) {
            const auto frames_read = handle_.readf(frame_buf, sf_count_t(sz) % FRAME_COUNT);

            // write channel data to destination
            for (sf_count_t j = 0; j < frames_read && frames_read_total < max_samples; j++) {
                x->w_float = frame_buf[j * n + ch] * gain();
                x++;
                frames_read_total++;
            }
        }

        return frames_read_total;
    }

    long LibSndFile::write(const t_word** src, size_t len, SoundFileFormat fmt, size_t num_ch, int samplerate)
    {
        // clang-format off
        int sf_fmt = 0;
        switch (fmt) {
        case FORMAT_WAV:  sf_fmt = SF_FORMAT_WAV;  break;
        case FORMAT_AIFF: sf_fmt = SF_FORMAT_AIFF; break;
        case FORMAT_OGG:  sf_fmt = SF_FORMAT_OGG; break;
        case FORMAT_OPUS: sf_fmt = SF_FORMAT_OPUS; break;
        case FORMAT_RAW:  sf_fmt = SF_FORMAT_RAW | SF_FORMAT_PCM_32; break;
        case FORMAT_FLAC: sf_fmt = SF_FORMAT_FLAC | SF_FORMAT_PCM_24; break;
        default:
            LIB_ERR << fmt::format("[SNDFILE] unsupported format: {}", to_string(fmt));
            return -1;
        }
        // clang-format on

        if (!handle_.formatCheck(sf_fmt, num_ch, samplerate)) {
            LIB_ERR << fmt::format("[SNDFILE] invalid options for format {}: "
                                   "num_channels={} samplerate={}",
                to_string(fmt), num_ch, samplerate);
            return -1;
        }

        handle_ = SndfileHandle(fname_.c_str(), SFM_WRITE, sf_fmt, num_ch, samplerate);
        if (handle_.rawHandle() == 0) {
            LIB_ERR << fmt::format("[SNDFILE] error while opening \"{}\": {}", fname_, sf_strerror(0));
            return -1;
        }

        long nframes = 0;
        constexpr sf_count_t FRAME_COUNT = 256;
        const sf_count_t OUT_BUF_SIZE = FRAME_COUNT * num_ch;
        float frame_buf[OUT_BUF_SIZE];
        std::int64_t frame_idx = 0;

        for (size_t i = 0; i < len; i++) {
            frame_idx = i % FRAME_COUNT;
            // fill frame
            for (size_t j = 0; j < num_ch; j++)
                frame_buf[frame_idx + j] = src[j][i].w_float * gain();

            // frame buffer is full
            if (frame_idx + 1 == FRAME_COUNT) {
                frame_idx = -1;
                nframes += handle_.write(frame_buf, FRAME_COUNT);
            }
        }

        // write remaining frames
        if (frame_idx >= 0)
            nframes += handle_.write(frame_buf, frame_idx + 1);

        handle_.writeSync();
        handle_ = {};
        return nframes;
    }

    long LibSndFile::readResampled(t_word* dest, size_t sz, size_t ch, long offset, size_t max_samples)
    {
        if (resampleRatio() < 0.001)
            return -1;

        t_word* x = dest;
        const sf_count_t FRAME_COUNT = 256;
        const int n = channels();
        const sf_count_t IN_BUF_SIZE = FRAME_COUNT * n;
        float frame_buf[IN_BUF_SIZE];

        // move to beginning
        if (handle_.seek(offset, SEEK_SET) == -1)
            return -1;

        // SoxR
        soxr_error_t error;
        soxr_t soxr = soxr_create(
            1, resampleRatio(), n, /* Input rate, output rate, # of channels. */
            &error, /* To report any error during creation. */
            nullptr, nullptr, nullptr); /* Use configuration defaults.*/

        if (error) {
            soxr_delete(soxr);
            return -1;
        }

        const size_t OUT_FRAME_COUNT = std::round(FRAME_COUNT * resampleRatio());
        const size_t OUT_BUF_SIZE = std::round(IN_BUF_SIZE * resampleRatio());
        float resampled_buf[OUT_BUF_SIZE];

        // read frames
        sf_count_t frames_read_total = 0;
        size_t frames_resampled_total = 0;
        const sf_count_t steps = sf_count_t(sz) / FRAME_COUNT;

        // read full buffers
        for (sf_count_t i = 0; i < steps; i++) {
            const auto frames_read = handle_.readf(frame_buf, FRAME_COUNT);
            if (frames_read == 0)
                break;

            /*
             * Copy data from the input buffer into the resampler, and resample
             * to produce as much output as is possible to the given output buffer:
             */
            size_t odone;
            error = soxr_process(soxr, frame_buf, frames_read, nullptr, resampled_buf, OUT_FRAME_COUNT, &odone);

            if (error) {
                std::cerr << "[soxr] error: " << error << "\n";
                break;
            }

            frames_read_total += frames_read;

            // write channel data to destination
            for (sf_count_t j = 0; j < odone && frames_resampled_total < max_samples; j++) {
                x->w_float = resampled_buf[j * n + ch] * gain();
                x++;
                frames_resampled_total++;
            }

            // seek to next
            if (handle_.seek(offset + frames_read_total, SEEK_SET) == -1)
                break;
        }

        // read remaining file samples
        if (sf_count_t(sz) % FRAME_COUNT != 0) {
            const sf_count_t frames_read = handle_.readf(frame_buf, sf_count_t(sz) % FRAME_COUNT);

            size_t odone;
            error = soxr_process(soxr, frame_buf, frames_read, nullptr, resampled_buf, OUT_FRAME_COUNT, &odone);

            if (!error) {
                // write channel data to destination
                for (sf_count_t j = 0; j < odone && frames_resampled_total < max_samples; j++) {
                    x->w_float = resampled_buf[j * n + ch] * gain();
                    x++;
                }
            } else
                std::cerr << "soxr: " << error << "\n";
        }

        // process remaining resample data
        while (true) {
            size_t odone = 0;
            // indicate end of input with nullptr
            error = soxr_process(soxr, nullptr, 0, nullptr, resampled_buf, OUT_FRAME_COUNT, &odone);
            if (error) {
                std::cerr << "[soxr] " << error << "\n";
                break;
            }

            if (odone == 0)
                break;

            // write channel data to destination
            for (sf_count_t j = 0; j < odone && frames_resampled_total < max_samples; j++) {
                x->w_float = resampled_buf[j * n + ch] * gain();
                x++;
                frames_resampled_total++;
            }
        }

        soxr_delete(soxr);

        return frames_resampled_total;
    }

    void LibSndFile::setLibOptions(int fmt, int nch, int sr)
    {
        user_format_ = fmt;
        user_channels_ = nch;
        user_samplerate_ = sr;
    }

    FormatList LibSndFile::supportedFormats()
    {
        FormatList res;

        SF_FORMAT_INFO format_info;
        int count;
        sf_command(0, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof(int));

        for (int k = 0; k < count; k++) {
            format_info.format = k;
            sf_command(0, SFC_GET_FORMAT_MAJOR, &format_info, sizeof(format_info));

            std::string finfo(format_info.name);
            std::string::size_type space_pos = finfo.find(' ', 0);
            std::string name = finfo.substr(0, space_pos);
            std::string desc = finfo.substr(space_pos + 0, std::string::npos);
            res.push_back(std::make_pair(name, desc));
        };

        return res;
    }
}
}
