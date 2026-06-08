import json
import math
from pathlib import Path
import matplotlib.pyplot as plt

JSON_DIR = Path("/home/andy/Desktop/Curtin/IRPLR/IRPLR/JSON")


def plot_solution(json_path: Path) -> None:
    with json_path.open() as f:
        sol = json.load(f)

    depot = (sol["metadata"]["depot_x"], sol["metadata"]["depot_y"])
    retailer_coords = {r["id"]: (r["x_coord"], r["y_coord"]) for r in sol["retailers"]}

    schedule = sol["schedule"]
    n_periods = len(schedule)
    if n_periods == 0:
        print(f"Skipping {json_path.name}: no schedule periods found.")
        return

    rows = 2 if n_periods > 1 else 1
    cols = math.ceil(n_periods / rows)

    fig, axes = plt.subplots(rows, cols, figsize=(6 * cols, 5 * rows))
    if n_periods == 1:
        axes = [axes]
    else:
        axes = axes.flatten()

    xs = [c[0] for c in retailer_coords.values()]
    ys = [c[1] for c in retailer_coords.values()]

    for i, period in enumerate(schedule):
        ax = axes[i]
        ax.scatter(xs, ys, c="lightgray", s=10)
        ax.scatter(depot[0], depot[1], c="red", marker="*", s=120)

        for veh in period["vehicles"]:
            route = veh["route"]
            if not route:
                continue

            coords = [depot] + [retailer_coords[r] for r in route] + [depot]
            x = [c[0] for c in coords]
            y = [c[1] for c in coords]
            ax.plot(x, y)

        ax.set_title(f"Period {period['time_period']}")

    for j in range(n_periods, rows * cols):
        fig.delaxes(axes[j])

    fig.tight_layout()
    out_path = Path(f"/home/andy/Desktop/Curtin/IRPLR/IRPLR/Plots/{json_path.stem}.png")
    fig.savefig(out_path, dpi=300, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved plot: {out_path}")


if __name__ == "__main__":
    json_files = sorted(JSON_DIR.glob("*.json"))
    if not json_files:
        raise SystemExit(f"No JSON files found in {JSON_DIR}")

    for json_path in json_files:
        plot_solution(json_path)
