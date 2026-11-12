"""Restore DBC section order after saving with CANdb Studio 0.3.0."""

import argparse
from pathlib import Path
import re


# Matches the section order accepted by the repository's dbcppp parser.
SECTIONS = (
    "VERSION", "NS_", "BS_", "BU_", "VAL_TABLE_", "BO_", "BO_TX_BU_",
    "EV_", "ENVVAR_DATA_", "SGTYPE_", "CM_", "BA_DEF_", "BA_DEF_DEF_",
    "BA_", "VAL_", "SIG_GROUP_", "SIG_VALTYPE_", "SG_MUL_VAL_",
)


def fix_order(data: bytes) -> bytes:
    text = data.decode("utf-8")
    # Indented SG_ definitions and NS_ keywords stay with their parent section.
    starts = list(re.finditer(r"(?m)^([A-Z][A-Z0-9_]*)(?=[\s:])", text))
    if not starts or starts[0].group(1) != "VERSION":
        raise ValueError("Expected a DBC starting with VERSION")
    blocks = []
    for index, match in enumerate(starts):
        keyword = match.group(1)
        if keyword not in SECTIONS:
            raise ValueError(f"Unsupported section {keyword}; file left unchanged")
        end = starts[index + 1].start() if index + 1 < len(starts) else len(text)
        blocks.append((SECTIONS.index(keyword), text[match.start():end]))
    blocks.sort(key=lambda block: block[0])
    newline = "\r\n" if "\r\n" in text else "\n"
    ordered = [block for _, block in blocks]
    for index in range(len(ordered) - 1):
        if not ordered[index].endswith("\n"):
            ordered[index] += newline
    return (text[:starts[0].start()] + "".join(ordered)).encode("utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("file", nargs="?", type=Path,
                        default=Path(__file__).resolve().parents[1] / "BYU_Racing_CAN.dbc")
    parser.add_argument("--check", action="store_true", help="Check without writing")
    args = parser.parse_args()
    original = args.file.read_bytes()
    try:
        fixed = fix_order(original)
    except (UnicodeError, ValueError) as error:
        parser.exit(1, f"{error}\n")
    if fixed == original:
        print(f"Section order OK: {args.file.name}")
    elif args.check:
        parser.exit(1, f"Sections need reordering: {args.file.name}\n")
    else:
        args.file.write_bytes(fixed)
        print(f"Fixed section order: {args.file.name}")


if __name__ == "__main__":
    main()
