# Big Data Task

Usage:

```bash
python3 calc_data.py <path_to_file>
```

`calc_data.py` — Python-обертка.
Основная обработка файла находится в `fast_calc.c` и собирается в `libfastcalc.so`.

Формат входа: бинарный файл из чисел `uint32` в big-endian.

Вывод:

```text
sum=...
min=...
max=...
```

## Docker

```bash
docker build -t calc-data .
docker run --rm -v "$PWD:/data" calc-data /data/input.bin
```

## Локальный запуск

```bash
make
python3 calc_data.py input.bin
```
