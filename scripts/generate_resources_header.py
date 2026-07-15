from __future__ import annotations

import os
import re
import sys
import unicodedata
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

print("")  # new line for CMake build comment

ROOT = Path(sys.argv[1])
OUTPUT = Path(sys.argv[2])

# Map each source folder to the C++ resource type name and the file extensions
# that should be treated as assets for that category.
RESOURCE_TYPES = {
    "images": ("Image", {".png", ".jpg", ".jpeg", ".bmp"}),
    "svg": ("SVG", {".svg"}),
    "fonts": ("Font", {".ttf", ".otf"}),
    "sounds": ("Sound", {".wav", ".mp3", ".ogg"}),
}

# C++ keywords must be disambiguated so generated identifiers remain valid
# symbols in the emitted header.
CPP_KEYWORDS = {
    "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel",
    "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor",
    "bool", "break", "case", "catch", "char", "char8_t", "char16_t",
    "char32_t", "class", "compl", "concept", "const", "consteval",
    "constexpr", "constinit", "const_cast", "continue", "co_await",
    "co_return", "co_yield", "decltype", "default", "delete", "do",
    "double", "dynamic_cast", "else", "enum", "explicit", "export",
    "extern", "false", "float", "for", "friend", "goto", "if", "inline",
    "int", "long", "mutable", "namespace", "new", "noexcept", "not",
    "not_eq", "nullptr", "operator", "or", "or_eq", "private",
    "protected", "public", "register", "reinterpret_cast", "requires",
    "return", "short", "signed", "sizeof", "static", "static_assert",
    "static_cast", "struct", "switch", "template", "this",
    "thread_local", "throw", "true", "try", "typedef", "typeid",
    "typename", "union", "unsigned", "using", "virtual", "void",
    "volatile", "wchar_t", "while", "xor", "xor_eq",
}

WINDOWS_RESERVED_NAMES = {
    "con", "prn", "aux", "nul",
    *(f"com{i}" for i in range(1, 10)),
    *(f"lpt{i}" for i in range(1, 10)),
}


@dataclass(slots=True)
class Resource:
    resource_type: str
    source_path: Path
    relative_path: str
    identifier: str
    extension: str


def emit(lines: list[str], *text: str) -> None:
    lines.extend(text)


def warning(message: str) -> None:
    print(f"Warning: {message}", file=sys.stderr)


def error(message: str) -> None:
    print(f"Error: {message}", file=sys.stderr)
    raise SystemExit(1)


def normalize_name(name: str) -> str:
    """
    Normalize Unicode so files that only differ by presentation form are treated
    as the same path internally.
    """
    return unicodedata.normalize("NFC", name)


def sanitize_identifier(name: str) -> str:
    """
    Convert an arbitrary asset name into a valid C++ identifier by replacing
    non-identifier characters and avoiding reserved words.
    """

    name = normalize_name(name)

    name = re.sub(r"\W", "_", name)

    name = re.sub(r"_+", "_", name)

    name = name.strip("_")

    if not name:
        name = "resource"

    if name[0].isdigit():
        name = "_" + name

    if name in CPP_KEYWORDS:
        name += "_"

    return name


def make_identifier(relative: Path) -> str:
    """
    Convert a relative asset path into a stable identifier for the generated
    C++ symbol. For example, ui/button/play.png becomes ui__button__play.
    """

    joined = "__".join(relative.with_suffix("").parts)

    return sanitize_identifier(joined)


def make_identifier_with_extension(relative: Path) -> str:
    """
    Build an identifier that preserves the file extension when multiple files
    would otherwise resolve to the same base identifier.
    """

    stem = relative.with_suffix("")
    extension = relative.suffix.lower().lstrip(".")

    joined = "__".join(stem.parts) + "_" + extension

    return sanitize_identifier(joined)


def print_collision(identifier: str, paths: list[Path]) -> None:
    error(
        "\n"
        f'Identifier collision detected: "{identifier}"\n\n'
        "Generated from:\n"
        + "".join(f"    - {p.as_posix()}\n" for p in paths)
        + "\nRename one of these files or directories."
    )


def check_windows_reserved(identifier: str) -> None:
    if identifier.lower() in WINDOWS_RESERVED_NAMES:
        warning(
            f'"{identifier}" is a reserved Windows filename. '
            "The generated code is fine, but the original asset "
            "may cause portability issues."
        )


def check_long_identifier(identifier: str) -> None:
    if len(identifier) > 128:
        warning(
            f'Identifier "{identifier}" is {len(identifier)} characters long.'
        )


def collect_resources(
        root: Path,
        output: Path,
        folder: str,
        resource_type: str,
        extensions: set[str],
) -> list[Resource]:
    """
    Discover all resources of a given type and validate the generated
    identifiers before returning them.
    """

    resource_dir = root / folder

    if not resource_dir.exists():
        return []

    files: list[Path] = []

    for file in sorted(resource_dir.rglob("*")):
        if not file.is_file():
            continue

        if file.suffix.lower() not in extensions:
            continue

        files.append(file)

    if not files:
        return []

    #
    # Pass 1: derive a base identifier for each file from its path.
    #
    # This groups files that would otherwise share a symbol name, such as:
    #   play.png
    #   play.jpg
    #
    # which become:
    #   play_png
    #   play_jpg
    #

    by_base: dict[str, list[Path]] = defaultdict(list)

    for file in files:
        relative = file.relative_to(resource_dir)
        key = make_identifier(relative)
        by_base[key].append(file)

    identifiers: dict[Path, str] = {}

    for base, group in by_base.items():
        extensions_in_group = {f.suffix.lower() for f in group}
        if len(group) > 1 and len(extensions_in_group) > 1:

            warning(
                f'Multiple {resource_type} resources resolve to "{base}" '
                "with different extensions. Appending extensions."
            )

            for file in group:
                relative = file.relative_to(resource_dir)
                identifiers[file] = make_identifier_with_extension(relative)

        elif len(group) > 1:

            print_collision(base, group)

        else:

            file = group[0]
            relative = file.relative_to(resource_dir)
            identifiers[file] = make_identifier(relative)

    collisions: dict[str, list[Path]] = defaultdict(list)

    for file, identifier in identifiers.items():
        collisions[identifier].append(file)

    for identifier, group in collisions.items():
        if len(group) > 1:
            print_collision(identifier, group)

    #
    # Pass 2: detect case-insensitive collisions.
    #
    # These are ambiguous on case-insensitive filesystems such as Windows and
    # must be rejected before code generation.
    #

    case_collisions: dict[str, list[Path]] = defaultdict(list)

    for file, identifier in identifiers.items():
        case_collisions[identifier.lower()].append(file)

    for group in case_collisions.values():

        unique = {identifiers[f] for f in group}

        if len(unique) > 1:
            error(
                "\nCase-insensitive identifier collision:\n\n"
                + "".join(
                    f"    {identifiers[f]} <- {f.relative_to(resource_dir).as_posix()}\n"
                    for f in group
                )
                + "\nThese identifiers differ only by letter case."
            )

    seen_paths: set[str] = set()

    for file in files:

        rel = normalize_name(
            file.relative_to(resource_dir).as_posix()
        )

        if rel in seen_paths:
            error(f'Duplicate resource path "{rel}".')

        seen_paths.add(rel)

    #
    # Pass 3: build the final Resource objects and validate the generated
    # identifiers for portability and length.
    #

    resources: list[Resource] = []

    for file in files:
        identifier = identifiers[file]

        check_windows_reserved(identifier)
        check_long_identifier(identifier)

        relative_output = Path(
            os.path.relpath(file, output.parent)
        ).as_posix()

        resources.append(
            Resource(
                resource_type=resource_type,
                source_path=file,
                relative_path=relative_output,
                identifier=identifier,
                extension=file.suffix.lower(),
            )
        )

    return resources


def emit_namespace(
        lines: list[str],
        resource_type: str,
        resources: list[Resource],
) -> None:
    """
    Emit the embedded byte-array definitions for one resource category.
    """

    emit(
        lines,
        f"    namespace {resource_type.lower()}_data {{",
    )

    for resource in resources:
        emit(
            lines,
            f"        inline constexpr unsigned char {resource.identifier}[] = {{",
            f'            #embed "{resource.relative_path}"',
            "        };",
            "",
        )

    emit(
        lines,
        "    }",
        "",
    )


def emit_enum(
        lines: list[str],
        resource_type: str,
        resources: list[Resource],
) -> None:
    """
    Emit the enum that gives each resource a stable identifier in C++.
    """

    emit(
        lines,
        f"    enum class {resource_type}ID {{",
    )

    for resource in resources:
        emit(
            lines,
            f"        {resource.identifier},",
        )

    emit(
        lines,
        "",
        "        Count",
        "    };",
        "",
    )


def emit_struct(
        lines: list[str],
        resource_type: str,
) -> None:
    """
    Emit the lightweight descriptor type used to describe a resource at runtime.
    """

    emit(
        lines,
        f"    struct {resource_type}Resource",
        "    {",
        f"        using IDType = {resource_type}ID;",
        "",
        "        IDType id;",
        "        const unsigned char* bytes;",
        "        size_t size;",
        "    };",
        "",
    )


def manifest_entry(resource: Resource) -> str:
    """
    Build a single manifest entry that describes how a resource is exposed to
    the consuming code.
    """

    namespace = resource.resource_type.lower()

    return (
        f"        {resource.resource_type}Resource {{ "
        f".id = {resource.resource_type}ID::{resource.identifier}, "
        f".bytes = {namespace}_data::{resource.identifier}, "
        f".size = sizeof({namespace}_data::{resource.identifier}) "
        f"}},"
    )


def emit_manifest(
        lines: list[str],
        all_resources: dict[str, list[Resource]],
) -> None:
    """
    Emit the variant alias and the flattened manifest that can be indexed at
    runtime.
    """

    resource_types = list(all_resources.keys())

    variant_types = ", ".join(
        f"{resource_type}Resource"
        for resource_type in resource_types
    )

    manifest = [
        manifest_entry(resource)
        for resources in all_resources.values()
        for resource in resources
    ]

    emit(
        lines,
        f"    using ResourceVariant = std::variant<{variant_types}>;",
        "",
        f"    inline constexpr std::array<ResourceVariant, {len(manifest)}> asset_manifest = {{",
    )

    lines.extend(manifest)

    emit(
        lines,
        "    };",
        "",
    )


def generate_header(
        all_resources: dict[str, list[Resource]],
) -> str:
    """
    Generate the complete C++ header that embeds all discovered assets.
    """

    lines: list[str] = []

    emit(
        lines,
        "// This file is auto-generated by 'generate_resources_header.py'",
        "// Do not edit this file directly.",
        "",
        "#pragma once",
        "",
        "#include <array>",
        "#include <variant>",
        "",
        "namespace dawn::resources {",
        "",
    )

    #
    # Embedded data blocks for each resource category.
    #

    for resource_type, resources in all_resources.items():
        emit_namespace(lines, resource_type, resources)

    #
    # Stable enum values for each generated resource.
    #

    for resource_type, resources in all_resources.items():
        emit_enum(lines, resource_type, resources)

    #
    # Runtime descriptors that point to the embedded bytes and size.
    #

    for resource_type in all_resources:
        emit_struct(lines, resource_type)

    #
    # Flattened manifest used by the consuming code to access resources.
    #

    emit_manifest(lines, all_resources)

    emit(
        lines,
        "}",
        "",
    )

    return "\n".join(lines)


def main() -> None:
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)

    all_resources: dict[str, list[Resource]] = {}

    #
    # Collect resources from each configured folder, validate their generated
    # identifiers, and group them by resource type.
    #
    for folder, (resource_type, extensions) in RESOURCE_TYPES.items():

        resources = collect_resources(
            ROOT,
            OUTPUT,
            folder,
            resource_type,
            extensions,
        )

        if resources:
            all_resources[resource_type] = resources

    #
    # Generate the header once all resource groups have been assembled.
    #
    header = generate_header(all_resources)

    OUTPUT.write_text(
        header,
        encoding="utf-8",
        newline="\n",
    )

    total_resources = sum(
        len(resources)
        for resources in all_resources.values()
    )

    print(
        f"Generated {OUTPUT} "
        f"({total_resources} resources, "
        f"{len(all_resources)} resource types)"
    )


if __name__ == "__main__":
    main()
