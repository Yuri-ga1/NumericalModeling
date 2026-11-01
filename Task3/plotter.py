from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

files_folder = Path("files")
graphs_folder = Path("graphs")

tasks = ["magnetic", "crossed", "gradient"]
particles = ["electron", "proton"]
plots = [
    ("trajectory", "x", "y", "x", "y"),
    ("energy", "t", "rel_total_error", "t", "(H-H0)/K0"),
    ("kinetic", "t", "rel_kinetic_change", "t", "(K-K0)/K0"),
]


def plot_experiment(task, particle, plot):
    name, x, y, xlabel, ylabel = plot
    plt.figure(figsize=(10, 8))

    for filename in sorted(files_folder.glob(f"{task}_{particle}_*.csv")):
        data = pd.read_csv(filename)
        method = "Явная" if "simple" in filename.stem else "Борис"
        dt = int(filename.stem.split("_dt")[-1]) / 1000
        plt.plot(data[x], data[y], label=f"{method}, dt/T={dt:g}")

    if name == "trajectory":
        theory_file = files_folder / f"{task}_{particle}_boris_dt20.csv"
        theory = pd.read_csv(theory_file)
        plt.plot(theory["x_theory"], theory["y_theory"], "k--", label="Теория")

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(f"{task}: {particle}")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(graphs_folder / f"{task}_{particle}_{name}.png")
    plt.close()


def main():
    graphs_folder.mkdir(parents=True, exist_ok=True)
    for task in tasks:
        for particle in particles:
            for plot in plots:
                if task == "magnetic" and plot[0] == "kinetic":
                    continue
                plot_experiment(task, particle, plot)


if __name__ == "__main__":
    main()
