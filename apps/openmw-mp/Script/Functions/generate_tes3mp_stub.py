import re
from pathlib import Path

input_dir = "./"
output_file = "tes3mp_api.lua"
indent = "               "  # 15 spaces

# Regex to handle multi-word return types like 'unsigned int'
func_decl_pattern = re.compile(
    r'(?:/\*\*(.*?)\*/)?\s*static\s+(?:(?:const\s+)?(?:unsigned\s+)?\w+(?:\s*\*?\s*)*)(\w+)\s*\(([^)]*)\)\s*(?:noexcept)?\s*;', re.DOTALL)

def format_comment_block(raw_comment):
    if not raw_comment:
        return f"--{indent}"
    lines = raw_comment.strip().split('\n')
    formatted = []
    for line in lines:
        line = line.strip().lstrip('*').rstrip()
        if not line:
            formatted.append(f"--{indent}")
            continue
        if line.startswith("\\param"):
            match = re.match(r"\\param\s+(\w+)\s*(.*)", line)
            if match:
                param, desc = match.groups()
                line = f"\\param {param}: {desc}"
        formatted.append(f"--  {line}{indent}")
    return "\n".join(formatted)

def cpp_to_lua_type(cpp_type, param_name):
    cpp_type = cpp_type.replace("const", "").replace("&", "").replace("*", "").strip()
    if param_name == "pid":
        return "number"
    mapping = {
        "int": "number", "float": "number", "double": "number", "bool": "boolean",
        "char": "string", "char*": "string", "std::string": "string", "std::string_view": "string",
        "size_t": "number", "unsigned int": "number", "unsigned short": "number",
        "short": "number", "long": "number", "void": "void"
    }
    return mapping.get(cpp_type, "any")

functions = []

# Parse all .hpp files in current directory
hpp_files = list(Path(input_dir).rglob("*.hpp"))

# Add ScriptFunctions.hpp from parent directory
script_functions_path = Path("../ScriptFunctions.hpp")
if script_functions_path.exists():
    hpp_files.append(script_functions_path)

# Parse each .hpp file
for hpp in hpp_files:
    text = hpp.read_text(errors='ignore')
    for match in func_decl_pattern.finditer(text):
        raw_comment, name, params = match.groups()
        comment = format_comment_block(raw_comment)
        param_list = []

        for p in params.split(','):
            p = p.strip()
            if not p:
                continue
            parts = p.replace("const ", "").strip().split()
            if len(parts) >= 2:
                param_name = parts[-1].replace("&", "").replace("*", "")
                cpp_type = " ".join(parts[:-1])
            else:
                param_name = parts[0]
                cpp_type = "any"
            lua_type = cpp_to_lua_type(cpp_type, param_name)
            param_list.append(f"{param_name}: {lua_type}")

        param_str = ", ".join(param_list)
        functions.append((name, comment, param_str))

# Generate Lua output
lines = [
    "---@meta",
    "-------------------------------------------------------------------------------",
    "--- TES3MP types",
    "-------------------------------------------------------------------------------",
    "",
    "-------------------------------------------------------------------------------",
    "--- tes3mp",
    "-------------------------------------------------------------------------------",
    "",
    "---@type TES3MP",
    "tes3mp = tes3mp",
    "",
    "---@class TES3MP"
]

for name, comment, param_str in sorted(functions):
    lines.append(comment)
    lines.append(f"---@field {name} fun({param_str})")

with open(output_file, "w", encoding="utf-8") as f:
    f.write("\n".join(lines))

print(f"✅ Generated {output_file} with {len(functions)} functions.")
