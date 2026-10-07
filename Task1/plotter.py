import pandas as pd
import matplotlib.pyplot as plt

filename = "integration_result.csv"

columns = [
    "N",
    "rectangle_time_us",
    "rectangle_error",
    "monte_carlo_time_us",
    "monte_carlo_error",
]

df = pd.read_csv(
    filename,
    sep=",",
    header=None,
    names=columns,
)

df = df.apply(pd.to_numeric, errors="coerce").dropna()

fig, axes = plt.subplots(2, 1, figsize=(10, 8))

axes[0].plot(
    df["N"], df["rectangle_time_us"], label="Метод прямоугольников"
)
axes[0].plot(
    df["N"], df["monte_carlo_time_us"], label="Метод Монте-Карло"
)
axes[0].set_ylabel("Время, мкс")
axes[0].set_title("Время вычислений")


axes[1].plot(
    df["N"], df["rectangle_error"] * 100,
    label="Метод прямоугольников",
)
axes[1].plot(
    df["N"], df["monte_carlo_error"] * 100,
    label="Метод Монте-Карло",
)
axes[1].set_yscale("log")
axes[1].set_ylabel("Относительная ошибка, %")
axes[1].set_xlabel("N")

for ax in axes:
    ax.grid(True, alpha=0.3)
    ax.legend()

fig.tight_layout()
fig.savefig("integration_plots.png")