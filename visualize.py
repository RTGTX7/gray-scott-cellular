import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LinearSegmentedColormap
import sys
import time

# Load the CSV data
file_path = 'grid_log_SIR.csv'
data = pd.read_csv(file_path, delimiter=';', low_memory=False)

# Remove rows where 'port_name' is populated
data = data[data['port_name'].isnull()]

# Extract the unique times for animation frames
times = data['time'].unique()

# Grid dimensions and origin
grid_size = (1000, 1000, 3)
origin = (0, 0)

# Initialize the grid
grid = np.zeros(grid_size)

# Create a custom colormap
colors = [(1, 1, 1), (1, 0, 0), (0, 1, 0)]  # White for S, Red for I, Green for R
n_bins = [0, 0.5, 1]  # Start, Midpoint, End
cmap_name = 'SIR_Colormap'
SIR_cmap = LinearSegmentedColormap.from_list(cmap_name, list(zip(n_bins, colors)))

#Set axis time
def seconds_to_hms_ms(seconds):
    hours, remainder = divmod(seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    # Split seconds into whole seconds and milliseconds
    seconds_whole, milliseconds = divmod(seconds, 1)
    # Format milliseconds to desired precision, e.g., 3 digits
    milliseconds = int(milliseconds * 1000)
    return f"{int(hours):02d}:{int(minutes):02d}:{int(seconds_whole):02d}.{milliseconds:03d}"


# Progress bar animation
def progress_bar(current, total, bar_length=50):
    fraction = current / total
    arrow = int(fraction * bar_length - 1) * '-' + '>'
    padding = int(bar_length - len(arrow)) * ' '
    ending = '\n' if current == total else '\r'
    sys.stdout.write(f'Progress: [{arrow}{padding}] {int(fraction*100)}%{ending}')
    sys.stdout.flush()

# Update function for animation
def update(frame_num):
    frame_data = data[data['time'] == times[frame_num]]
    for _, row in frame_data.iterrows():
        x, y = map(int, row['model_name'].strip('()').split(','))
        population, s, i, r = map(float, row['data'].strip('<>').split(','))
        # Normalize the S, I, R values to range [0, 1] for the colormap
        total = s + i + r
        if total > 0:
            # color_val = (i/total, r/total, s/total)  # Assuming RGB corresponds to I, R, S
            color_val = (i, r, s)
        else:
            color_val = (0, 0, 0)  # Default to black if no population
        grid[y - origin[1], x - origin[0]] = color_val  # Store the color value directly
    mat.set_data(grid)  # Update with the new grid of color values
    ax.set_title(f"Time {seconds_to_hms_ms(times[frame_num])}")
    progress_bar(frame_num + 1, len(times))
    return [mat]

# Set up the plot
fig, ax = plt.subplots()
mat = ax.matshow(grid, cmap=SIR_cmap)
plt.colorbar(mat, label='State Gradient')
ax.set_title("Time 00:00:00:000")

print('Initializing animation')
# Create the animation
ani = animation.FuncAnimation(fig, update, frames=len(times), interval=100)

print('Saving the animation')
# Save the animation as a video file
ani.save('SIR_animation_2.mp4', writer='ffmpeg', dpi=300)

# If you want to display the animation inline (in a Jupyter notebook, for example), use:
# from IPython.display import HTML
# HTML(ani.to_html5_video())