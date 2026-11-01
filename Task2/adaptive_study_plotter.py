import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
import math

def plot_eps(filepath, graphs_folder):
    eps = pd.read_csv(filepath)

    fig, axes = plt.subplots(2, 1, figsize=(10, 8))

    axes[0].loglog(eps["eps"], eps["adaptive_error"], marker="o")
    axes[0].set_ylabel("Относительная ошибка")
    axes[0].set_title("Сравнение с аналитическим решением")

    axes[1].semilogx(eps["eps"], eps["adaptive_time_us"], marker="o")
    axes[1].set_xlabel("eps")
    axes[1].set_ylabel("Время, мкс")
    axes[1].set_title("Скорость вычисления")

    for ax in axes:
        ax.grid(True, alpha=0.3)

    fig.tight_layout()
    fig.savefig(graphs_folder / "adaptive_eps_plots.png")
    plt.close(fig)

def plot_N(filepath, graphs_folder):
    n = pd.read_csv(filepath)
    fig, axes = plt.subplots(2, 1, figsize=(10, 8))
    oscillating_exact = (1 - math.cos(300)) / 30

    axes[0].plot(n["N"], n["adaptive_integral"], label="Адаптивный метод")
    axes[0].axhline(oscillating_exact, linestyle="--", label="Аналитическое решение")
    axes[0].set_ylabel("Значение интеграла")
    axes[0].legend()

    axes[1].semilogy(n["N"], n["absolute_error"])
    axes[1].set_xlabel("Количество разбиений N")
    axes[1].set_ylabel("Абсолютная ошибка")


    for ax in axes:
        ax.grid(True, alpha=0.3)

    fig.tight_layout()
    fig.savefig(graphs_folder / "adaptive_N_plots.png")
    plt.close(fig)

if __name__ == '__main__':
    graphs_folder = Path("graphs")
    graphs_folder.mkdir(parents=True, exist_ok=True)

    eps_filepath = Path("files/adaptive_eps_result.csv")
    N_filepath = Path("files/adaptive_N_result.csv")

    plot_eps(eps_filepath, graphs_folder)
    plot_N(N_filepath, graphs_folder)