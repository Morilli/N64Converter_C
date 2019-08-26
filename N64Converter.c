#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <byteswap.h>
#include <assert.h>


int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("N64Converter - Converts any N64 rom from [.n64|.v64|.z64] to .z64.\n");
        printf("Usage: ./N64Converter path/to/input.[n64|v64|z64] path/to/output.z64\n");
        exit(EXIT_SUCCESS);
    }

    FILE* input = fopen(argv[1], "rb");
    if (input == NULL) {
        fprintf(stderr, "Error: Couldn't open input file. Make sure it exists and is accessable.\n");
        exit(EXIT_FAILURE);
    }

    fseek(input, 0, SEEK_END);
    const long input_length = ftell(input);
    rewind(input);
    uint8_t* buffer = malloc(input_length);
    assert(fread(buffer, 1, input_length, input) == input_length);

    fclose(input);

    char input_format[5];
    /* Test if rom is a native .z64 image with header 0x80371240. [ABCD] */
    if (memcmp(buffer, "\x80\x37\x12\x40", 4) == 0) {
        memcpy(input_format, ".z64", 5);
        // no conversion done, simple copy
    }
    /* Test if rom is a byteswapped .v64 image with header 0x37804012. [BADC] */
    else if (memcmp(buffer, "\x37\x80\x40\x12", 4) == 0) {
        memcpy(input_format, ".v64", 5);
        for (int i = 0; i < input_length / 2; i++) {
            ((uint16_t*) buffer)[i] = bswap_16(((uint16_t*) buffer)[i]);
        }
    }
    /* Test if rom is a little-endian .n64 image with header 0x40123780. [DCBA] */
    else if (memcmp(buffer, "\x40\x12\x37\x80", 4) == 0) {
        memcpy(input_format, ".n64", 5);
        for (int i = 0; i < input_length / 4; i++) {
            ((uint32_t*) buffer)[i] = bswap_32(((uint32_t*) buffer)[i]);
        }
    } else {
        fprintf(stderr, "Error: Unrecognized input file format.\n");
        exit(EXIT_FAILURE);
    }

    FILE* output = fopen(argv[2], "wb");
    if (output == NULL) {
        fprintf(stderr, "Error: Couldn't open output file.\n");
        exit(EXIT_FAILURE);
    }

    fwrite(buffer, 1, input_length, output);
    free(buffer);
    fclose(output);

    printf("Successfully converted from %s to .z64 format.\n", input_format);
}
