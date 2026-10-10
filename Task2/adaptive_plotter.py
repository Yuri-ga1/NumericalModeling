import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

filename = Path("files/adaptive_points.csv")

columns = [
    "x",
    "dx",
    "f(x)"
]

df = pd.read_csv(
    filename,
    sep=",",
    header=None,
    names=columns,
)

df.sort_values("x")
df = df.apply(pd.to_numeric, errors="coerce").dropna()

plt.figure(figsize=(10, 8))

plt.bar(
    df["x"], df["f(x)"], df["dx"],
)

plt.xlabel("x")
plt.ylabel("f(x)")

plt.grid(True, alpha=0.3)

graphs_folder = Path("graphs")
graphs_folder.mkdir(parents=True, exist_ok=True)


plt.tight_layout()
plt.savefig(graphs_folder / "adaptive_plots.png")