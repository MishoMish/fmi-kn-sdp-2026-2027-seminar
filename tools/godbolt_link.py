#!/usr/bin/env python3
"""Builds a Compiler Explorer (godbolt.org) link that opens with a given
source file and compilers already set up. Nothing is uploaded: the whole
state is encoded in the URL itself (godbolt's /clientstate/ format).

    python3 tools/godbolt_link.py file.cpp g151:-O0 clang1810:-O2

Compiler ids: g151 = x86-64 GCC 15.1, clang1810 = x86-64 Clang 18.1.0
(see https://godbolt.org/api/compilers/c++ for all ids).
"""

import base64
import json
import sys
from pathlib import Path


def link(source, compilers):
    state = {
        "sessions": [{
            "id": 1,
            "language": "c++",
            "source": source,
            "compilers": [{"id": cid, "options": opts} for cid, opts in compilers],
        }]
    }
    encoded = base64.b64encode(json.dumps(state).encode("utf-8")).decode("ascii")
    return "https://godbolt.org/clientstate/" + encoded


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 1
    source = Path(argv[1]).read_text(encoding="utf-8")
    compilers = [tuple(arg.split(":", 1)) for arg in argv[2:]]
    print(link(source, compilers))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
