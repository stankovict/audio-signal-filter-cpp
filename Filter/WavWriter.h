#ifndef WAVWRITER_H
#define WAVWRITER_H

#include <vector>
#include <string>
#include <cstdint>

/*
 * WavWriter
 * This class is used to save an audio signal into a WAV file.
 * Comments more detailed for better personal understanding as this is a new topic for me.
 * 
 * Important notes about WAV:
 * - WAV is a very simple binary file format.
 * - It starts with a header (metadata), followed by raw audio samples.
 * - In this project we use:
 *     - 44.1 kHz sample rate
 *     - mono sound (1 channel)
 *     - 16-bit PCM samples
 * As far as I understand, this is a common format for uncompressed audio.
 *
 * The input signal is a vector of doubles in range [-1.0, 1.0].
 * These values are converted to 16-bit integers before saving.
 */
class WavWriter {
private:
    /*
     * Audio format constants.
     * These values are written into the WAV header.
     */

     // Number of samples per second (CD quality)
	static constexpr uint32_t SAMPLE_RATE = 44100; // 44.1 kHz, standard for audio CDs, default sample rate for high-quality audio

    // Mono audio (only one channel)
	static constexpr uint16_t NUM_CHANNELS = 1; //because we are dealing with mono audio, which has a single audio channel

    // Each sample is stored as a 16-bit signed integer
	static constexpr uint16_t BITS_PER_SAMPLE = 16; //16 bits per sample provides a good balance between audio quality and file size

    /*
     * Writes a 32-bit unsigned integer into a byte buffer
     * using little-endian byte order.
     *
     * WAV files always use little-endian format.
     * Example:
     *   value = 0x12345678
     *   written as bytes: 78 56 34 12
     *
     */
    static void writeInt32(std::vector<uint8_t>& buf, uint32_t val);

    /*
     * Writes a 16-bit unsigned integer into a byte buffer
     * using little-endian byte order.
     */
    static void writeInt16(std::vector<uint8_t>& buf, uint16_t val);

    /*
     * Creates a standard WAV file header.
     *
     * dataSize = number of bytes of audio data (NOT samples).
     *
     * The header contains:
     * - "RIFF" chunk
     * - "WAVE" format identifier
     * - "fmt " subchunk (audio format info)
     * - "data" subchunk (audio data size)
     *
     * The header is always 44 bytes long for PCM WAV files.
     */
    static std::vector<uint8_t> createHeader(uint32_t dataSize);

    /*
     * Converts a floating-point audio sample to 16-bit PCM.
     *
     * Input range:
     *   -1.0 -> minimum sample value
     *    0.0 -> silence
     *    1.0 -> maximum sample value
     *
     * Output range:
     *   -32768 .. 32767
     *
     * Values outside [-1, 1] should be clipped.
     */
    static int16_t doubleToPCM16(double val);

public:
    /*
     * Writes an audio signal to a WAV file.
     *
     * filename - name of the output WAV file
     * signal   - vector of samples in range [-1, 1]
     *
     * Steps performed inside this function:
     * 1. Check that the signal is not empty
     * 2. Convert all samples to 16-bit PCM
     * 3. Create a WAV header
     * 4. Write header + audio data to file
     *
     * Returns:
     * - true  if file was written successfully
     * - false if an error occurred
     */
    static bool writeWav(const std::string& filename,
        const std::vector<double>& signal);
};

#endif