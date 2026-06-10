#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define U32_SIZE 4 //4 байта
#define BUFFER_SIZE (64u * 1024u * 1024u) //64 мб
#define BIG_BASE 1000000000u //по 9 цифр для длинной арифметики суммы
#define BIG_CAP 64 //макс кол-во блоков  в BigInt

typedef struct {
    uint32_t digits[BIG_CAP];
    size_t len;
} BigInt;

void bigint_init(BigInt *num) {
    num->digits[0] = 0;
    num->len = 1;
}

int bigint_add_u32(BigInt *num, uint32_t value) {
    uint64_t carry = value;
    size_t i = 0;

    while (carry > 0) {
        if (i == num->len) {
            if (num->len == BIG_CAP) {
                return 1;
            }
            num->digits[num->len] = 0;
            num->len++;
        }

        uint64_t current = (uint64_t)num->digits[i] + carry;
        num->digits[i] = (uint32_t)(current % BIG_BASE);
        carry = current / BIG_BASE;
        i++;
    }

    return 0;
}

int bigint_to_string(const BigInt *num, char *buffer, size_t buffer_size) {
    int written = snprintf(buffer, buffer_size, "%u", num->digits[num->len - 1]); //печатаем самый старший блок
    if (written < 0 || (size_t)written >= buffer_size) {
        return 1;
    }

    size_t used = (size_t)written;

    for (size_t i = num->len - 1; i-- > 0;) {
        written = snprintf(buffer + used, buffer_size - used, "%09u", num->digits[i]); //дополняем нестаршие блоки нулями слева, если меньше 9 цифр
        if (written < 0 || (size_t)written >= buffer_size - used) { //поместились в буфер?
            return 1;
        }
        used += (size_t)written;
    }

    return 0;
}

typedef struct {
    char sum[128];
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

    BigInt sum; bigint_init(&sum);
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

                bigint_add_u32(&sum, value);

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

    bigint_to_string(&sum, out->sum, sizeof(out->sum));
    out->min = min_value;
    out->max = max_value;
    return 0;
}
