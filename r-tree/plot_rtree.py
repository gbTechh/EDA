# plot_rtree.py
import os, sys, glob, json
import matplotlib.pyplot as plt
import matplotlib.patches as patches

def plot_folder(folder):
    files = sorted(glob.glob(os.path.join(folder, "step_*.json")))
    if not files:
        print("No files found in", folder)
        return

    # Colores distintos por nivel del árbol
    level_colors = ['red', 'blue', 'orange', 'purple', 'brown']

    for f in files:
        with open(f, 'r') as fh:
            data = json.load(fh)

        all_pts = data.get("all_points", [])
        mbr = data.get("mbr", None)
        tree = data.get("tree", [])

        fig, ax = plt.subplots(figsize=(6,6))

        # Dibujar todos los puntos acumulados
        if all_pts:
            xs = [p[0] for p in all_pts]
            ys = [p[1] for p in all_pts]
            ax.scatter(xs, ys, c='black', s=20, zorder=5, label="Points")

        # Dibujar MBRs por nivel
        for lvl, level in enumerate(tree):
            col = level_colors[lvl % len(level_colors)]
            for rect in level:
                xmin = rect[0][0]; ymin = rect[0][1]
                xmax = rect[1][0]; ymax = rect[1][1]
                w = xmax - xmin; h = ymax - ymin
                patch = patches.Rectangle((xmin, ymin), w, h,
                                          fill=False, edgecolor=col,
                                          linewidth=1.2, alpha=0.7)
                ax.add_patch(patch)

        # Dibujar el MBR recién insertado en verde grueso
        if mbr:
            xmin, ymin, xmax, ymax = mbr
            w = xmax - xmin; h = ymax - ymin
            patch = patches.Rectangle((xmin, ymin), w, h,
                                      fill=False, edgecolor='green',
                                      linewidth=2.5, linestyle="--",
                                      label="Last inserted MBR")
            ax.add_patch(patch)

        ax.set_aspect('equal', adjustable='box')
        ax.set_title(os.path.basename(f))
        ax.legend(loc="upper left", fontsize=8)

        outpng = f.replace('.json', '.png')
        plt.savefig(outpng, dpi=150, bbox_inches='tight')
        plt.close(fig)
        print("Saved", outpng)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python3 plot_rtree.py outputs/MAXNODES_2_FIXED")
    else:
        plot_folder(sys.argv[1])
