#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <byteswap.h>


void fileswap_16(FILE* input, FILE* output)
{
    uint16_t current_uint16;
    while (fread(&current_uint16, 2, 1, input)) {
        current_uint16 = bswap_16(current_uint16);
        fwrite(&current_uint16, 2, 1, output);
    }
}

void fileswap_32(FILE* input, FILE* output)
{
    uint32_t current_uint32;
    while (fread(&current_uint32, 4, 1, input)) {
        current_uint32 = bswap_32(current_uint32);
        fwrite(&current_uint32, 4, 1, output);
    }
}

void fileswap_2_16(FILE* input, FILE* output)
{
    uint32_t current_uint32;
    while (fread(&current_uint32, 4, 1, input)) {
        current_uint32 = (current_uint32 >> 16 | current_uint32 << 16);
        fwrite(&current_uint32, 4, 1, output);
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("Usage: ./N64Converter path/to/input.[n64|v64|z64] path/to/output.[n64|v64|z64]\n");
        exit(EXIT_SUCCESS);
    }

    char* input_extension = &argv[1][strlen(argv[1]) - 3];
    char* output_extension = &argv[2][strlen(argv[2]) - 3];
    if (strcmp(input_extension, "n64") && strcmp(input_extension, "v64") && strcmp(input_extension, "z64")) {
        fprintf(stderr, "Error: Incorrect extension for input file: Has to be one of [n64, v64, z64], but was %s.\n", input_extension);
        exit(EXIT_FAILURE);
    } else if (strcmp(output_extension, "n64") && strcmp(output_extension, "v64") && strcmp(output_extension, "z64")) {
        fprintf(stderr, "Error: Incorrect extension for output file: Has to be one of [n64, v64, z64], but was %s.\n", output_extension);
        exit(EXIT_FAILURE);
    }

    FILE* input = fopen(argv[1], "rb");
    if (input == NULL) {
        fprintf(stderr, "Error: Couldn't open input file. Make sure it exists and is accessable.\n");
        exit(EXIT_FAILURE);
    }

    FILE* output = fopen(argv[2], "wb");
    if (output == NULL) {
        fprintf(stderr, "Error: Couldn't open output file.\n");
        exit(EXIT_FAILURE);
    }

    switch(*input_extension + *output_extension)
    {
        case 240: // v64 to z64 or vice-versa
            fileswap_32(input, output);
            break;
        case 232: // n64 to z64 or vice-versa
            fileswap_16(input, output);
            break;
        case 228: // n64 to v64 or vice-versa
            fileswap_2_16(input, output);
            break;
        default: { // no conversion done, simple copy
            int c;
            while ((c = fgetc(input)) != EOF) {
                putc(c, output);
            }
        }
    }

    printf("Successfully converted from %s to %s.\n", input_extension, output_extension);
}
