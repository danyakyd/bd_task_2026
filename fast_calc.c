#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define U32_SIZE 4 //4 байта
#define BUFFER_SIZE (64u * 1024u * 1024u) //64 мб

typedef struct {
    uint64_t sum; //т.к. uint32+uint32
    uint32_t min;
    uint32_t max;
} CalcResult;


int calc_file_stat(const char *path, CalcResult *out) {
    if (path == NULL || out == NULL) {
        return 2;
    }

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return 3;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return 4;
    }

    long file_size = ftell(file);
    if (file_size <= 0) {
        fclose(file);
        return 5;
    }


    if (fseek(file, 0, SEEK_SET) != 0) { //!=0 - ошибка
        fclose(file);
        return 6;
    }

    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        fclose(file);
        return 7;
    }

    uint64_t sum = 0;
    uint32_t min_value = UINT32_MAX;
    uint32_t max_value = 0;

    while (1) {
        size_t read_bytes = fread(buffer, 1, BUFFER_SIZE, file);

        if (read_bytes > 0) {
            for (size_t i = 0; i < read_bytes; i += U32_SIZE) {
                uint32_t value = //4 байта:
                    ((uint32_t)buffer[i] << 24) |
                    ((uint32_t)buffer[i + 1] << 16) |
                    ((uint32_t)buffer[i + 2] << 8) |
                    ((uint32_t)buffer[i + 3]);

                sum += value;
                if (value < min_value) {
                    min_value = value;
                }
                if (value > max_value) {
                    max_value = value;
                }
            }
        }

        if (read_bytes < BUFFER_SIZE) {
            if (ferror(file)) {
                free(buffer);
                fclose(file);
                return 9;
            }
            break;
        }
    }

    free(buffer);
    fclose(file);

    out->sum = sum;
    out->min = min_value;
    out->max = max_value;
    return 0;
}
