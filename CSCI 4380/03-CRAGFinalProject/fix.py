import json

in_path = "crag_light_final.ipynb"
out_path = "project_fixed.ipynb"

with open(in_path, "r", encoding="utf-8") as f:
    txt = f.read()

# Parse only the *first* JSON object (ignoring the broken tail)
dec = json.JSONDecoder()
nb_obj, end = dec.raw_decode(txt)

# Clean up metadata: keep only the "normal" keys
meta = nb_obj.get("metadata", {})
safe_keys = {"accelerator", "colab", "kernelspec", "language_info"}
nb_obj["metadata"] = {k: v for k, v in meta.items() if k in safe_keys}

# Restore nbformat fields (standard for v4 notebooks)
nb_obj["nbformat"] = 4
nb_obj["nbformat_minor"] = 5  # 4 or 5 is fine, GitHub just needs it present

# Write a clean notebook JSON
with open(out_path, "w", encoding="utf-8") as f:
    json.dump(nb_obj, f, indent=1)

print(f"Fixed notebook written to {out_path}")

