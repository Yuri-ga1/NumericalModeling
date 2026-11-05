from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

files_folder = Path("files")
graphs_folder = Path("graphs")

tasks = ["magnetic_Euler", "magnetic_Boris", "crossed", "gradient"]
particles = ["electron", "proton"]
colors = {20: "tab:blue", 100: "tab:orange", 500: "tab:green", 1000: "tab:red"}
plots = [
    ("trajectory", "x", "y", "x", "y"),
    ("energy", "t", "rel_total_error", "t", "(H-H0)/K0"),
    ("kinetic", "t", "rel_kinetic_change", "t", "(K-K0)/K0"),
]


def plot_group(particle, plot):
    name, x, y, xlabel, ylabel = plot
    figure, axes = plt.subplots(2, 2, figsize=(14, 10), layout="constrained")
    figure.suptitle(f"{particle}: {name}")

    for task, axis in zip(tasks, axes.flat):
        axis.set_title(task)
        filenames = sorted(
            files_folder.glob(f"{task}_{particle}_dt*.csv"),
            key=lambda path: int(path.stem.split("_dt")[-1]),
        )
        if (name == "kinetic" and task.startswith("magnetic")) or not filenames:
            axis.text(0.5, 0.5, "NO DATA", ha="center", va="center", transform=axis.transAxes, fontsize=18)
            axis.set_axis_off()
            continue

        for filename in filenames:
            data = pd.read_csv(filename)
            dt = int(filename.stem.split("_dt")[-1])
            axis.plot(data[x], data[y], color=colors.get(dt), linewidth=1.5, label=f"dt/T={dt / 1000:g}")
            if name == "trajectory" and dt == 20 and {"x_theory", "y_theory"}.issubset(data.columns):
                axis.plot(data["x_theory"], data["y_theory"], "k--", linewidth=1.5, label="Теория")

        axis.set_xlabel(xlabel)
        axis.set_ylabel(ylabel)
        axis.grid(True, alpha=0.3)
        axis.legend()
        if name == "trajectory":
            axis.set_aspect("equal", adjustable="datalim")

    figure.savefig(graphs_folder / f"{particle}_{name}.png", dpi=150)
    plt.close(figure)


def main():
    graphs_folder.mkdir(parents=True, exist_ok=True)
    for particle in particles:
        for plot in plots:
            plot_group(particle, plot)


if __name__ == "__main__":
    main()
