import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

files_folder = Path("files")
graphs_folder = Path("graphs")
graphs_folder.mkdir(parents=True, exist_ok=True)

tasks = ["magnetic", "crossed", "gradient"]
particles = ["electron", "proton"]

plots = [
    ("trajectory", "x", "y", "x", "y"),
    ("energy", "t", "rel_total_error", "t", "(H-H0)/K0"),
    ("kinetic", "t", "rel_kinetic_change", "t", "(K-K0)/K0"),
]

for task in tasks:
    for particle in particles:
        for name, x, y, xlabel, ylabel in plots:
            if task == "magnetic" and name == "kinetic":
                continue

            plt.figure(figsize=(10, 8))

            for filename in sorted(files_folder.glob(f"{task}_{particle}_*.csv")):
                df = pd.read_csv(filename)
                method = "Явная" if "simple" in filename.stem else "Борис"
                dt = int(filename.stem.split("_dt")[-1]) / 1000
                plt.plot(df[x], df[y], label=f"{method}, dt/T={dt:g}")

            if name == "trajectory":
                df = pd.read_csv(files_folder / f"{task}_{particle}_boris_dt20.csv")
                plt.plot(df["x_theory"], df["y_theory"], "k--", label="Теория")

            plt.xlabel(xlabel)
            plt.ylabel(ylabel)
            plt.title(f"{task}: {particle}")
            plt.grid(True, alpha=0.3)
            plt.legend()
            plt.tight_layout()
            plt.savefig(graphs_folder / f"{task}_{particle}_{name}.png")
            plt.close()
