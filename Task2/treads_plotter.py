import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

filename = ("files/treads_result.csv")

columns = [
	"threads",
	"rectangle_time_us",
	"monte_carlo_time_us",
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
	df["threads"], df["rectangle_time_us"], label="Метод прямоугольников"
)

axes[1].plot(
	df["threads"], df["monte_carlo_time_us"], label="Метод Монте-Карло"
)

for ax in axes:
	ax.set_ylabel("Время, мкс")
	ax.set_xlabel("Количество потоков")
	ax.set_title("Время вычислений")
	ax.grid(True, alpha=0.3)
	ax.legend()

graphs_folder = Path("graphs")
graphs_folder.mkdir(parents=True, exist_ok=True)

fig.tight_layout()
fig.savefig(graphs_folder / "treads_plot.png")