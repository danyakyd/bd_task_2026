import ctypes
import sys
from pathlib import Path


class CalcResult(ctypes.Structure):
    _fields_ = [
        ("sum", ctypes.c_uint64),
        ("min", ctypes.c_uint32),
        ("max", ctypes.c_uint32),
    ]


def load_library() -> ctypes.CDLL:
    lib_path = Path(__file__).with_name("libfastcalc.so")
    lib = ctypes.CDLL(str(lib_path))
    lib.calc_file_stat.argtypes = [
        ctypes.c_char_p,
        ctypes.POINTER(CalcResult)
    ]
    lib.calc_file_stat.restype = ctypes.c_int
    return lib


def calculate(file_path: str) -> CalcResult:
    lib = load_library()
    result = CalcResult()

    code = lib.calc_file_stat(
        file_path.encode("utf-8"),
        ctypes.byref(result)
    )

    if code != 0:
        raise RuntimeError(code)

    return result


def main(argv: list[str]) -> int:
    if len(argv) != 2:
        print(f"Use this way: {Path(argv[0]).name} <path_to_file>")
        return 2

    try:
        result = calculate(argv[1])
    except Exception as exc:
        print(f"error: {exc}")
        return 1

    print(f"sum={result.sum}")
    print(f"min={result.min}")
    print(f"max={result.max}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
