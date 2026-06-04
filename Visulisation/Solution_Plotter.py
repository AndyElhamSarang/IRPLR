import json
import math
import matplotlib.pyplot as plt

with open("initial_solution_0.json") as f:
    sol = json.load(f)

depot = (sol["metadata"]["depot_x"], sol["metadata"]["depot_y"])
retailer_coords = {r["id"]: (r["x_coord"], r["y_coord"]) for r in sol["retailers"]}

schedule = sol["schedule"]
n_periods = len(schedule)

# ✅ force horizontal layout
rows = 2
cols = math.ceil(n_periods / rows)

fig, axes = plt.subplots(rows, cols, figsize=(6 * cols, 5 * rows))
axes = axes.flatten()

for i, period in enumerate(schedule):
    ax = axes[i]

    xs = [c[0] for c in retailer_coords.values()]
    ys = [c[1] for c in retailer_coords.values()]
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

# remove unused axes
for j in range(n_periods, rows * cols):
    fig.delaxes(axes[j])

fig.tight_layout()
fig.savefig("solution_routes.png", dpi=300, bbox_inches="tight")
plt.show()