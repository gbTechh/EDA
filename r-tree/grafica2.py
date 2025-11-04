# plot_rtree_max2.py
import os, glob, json
import matplotlib.pyplot as plt
import matplotlib.patches as patches

def plot_folder(folder):
    files = sorted(glob.glob(os.path.join(folder, "step_*.json")))
    if not files:
        print("No files found in", folder)
        return

    level_colors = ['red', 'blue', 'green', 'purple', 'orange']

    for f in files:
        with open(f, 'r') as fh:
            data = json.load(fh)

        all_pts = data.get("all_points", [])
        mbr = data.get("mbr", None)
        tree = data.get("tree", [])

        fig, ax = plt.subplots(figsize=(6,6))

        # puntos insertados
        if all_pts:
            xs = [p[0] for p in all_pts]
            ys = [p[1] for p in all_pts]
            ax.scatter(xs, ys, c='black', s=8, zorder=5)

        # dibujar MBRs
        for lvl, level in enumerate(tree):
            col = level_colors[lvl % len(level_colors)]
            for ridx, rect in enumerate(level):
                xmin = rect[0][0]; ymin = rect[0][1]
                xmax = rect[1][0]; ymax = rect[1][1]
                w = xmax - xmin; h = ymax - ymin
                patch = patches.Rectangle((xmin, ymin), w, h,
                                          fill=False,
                                          edgecolor=col,
                                          linestyle="--" if lvl>0 else "-",
                                          linewidth=1.2+lvl*0.4,
                                          alpha=0.7)
                ax.add_patch(patch)
                # etiqueta en el centro
                ax.text((xmin+xmax)/2, (ymin+ymax)/2,
                        f"L{lvl}", ha="center", va="center",
                        fontsize=6, color=col)

        # rectángulo insertado en este paso
        if mbr:
            xmin, ymin, xmax, ymax = mbr
            w = xmax - xmin; h = ymax - ymin
            patch = patches.Rectangle((xmin, ymin), w, h,
                                      fill=False, edgecolor='green',
                                      linewidth=2.0)
            ax.add_patch(patch)

        ax.set_aspect('equal', adjustable='box')
        ax.set_title(f"MAXNODES=2 :: {os.path.basename(f)}")
        outpng = f.replace('.json', '.png')
        plt.savefig(outpng, dpi=150, bbox_inches='tight')
        plt.close(fig)
        print("Saved", outpng)

if __name__ == '__main__':
    plot_folder("outputs/MAXNODES_2")
