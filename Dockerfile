FROM python:3.14-slim AS build

WORKDIR /app
RUN apt-get update \
    && apt-get install -y --no-install-recommends gcc make \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

COPY fast_calc.c Makefile ./
RUN make

FROM python:3.14-slim

WORKDIR /app
COPY --from=build /app/libfastcalc.so ./libfastcalc.so
COPY calc_data.py ./calc_data.py

ENTRYPOINT ["python3", "/app/calc_data.py"]
