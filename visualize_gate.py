import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as patches
import sys

# Load the CSV data
file_path = 'grid_log.csv'
data = pd.read_csv(file_path, delimiter=';')

# Remove rows where 'port_name' is populated
data = data[data['port_name'].isnull()]

# Extract the unique times for animation frames
times = data['time'].unique()

# Grid dimensions and origin
grid_size = (20, 20, 3)
origin = (0, 0)

# Initialize the grid
grid = np.zeros(grid_size)

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
        state = int(row['data'].strip('<>').split(',')[0])
        color_val = (0, 0, 1)
        if(state == 1):
            color_val = (1, 1, 1)
        elif(state == 0):
            color_val = (0, 0, 0)

        grid[y - origin[1], x - origin[0]] = color_val  # Store the color value directly

    mat.set_data(grid)  # Update with the new grid of color values

    ax.set_title(f"Time {seconds_to_hms_ms(times[frame_num])}")
    progress_bar(frame_num + 1, len(times))
    return [mat]

# Set up the plot
fig, ax = plt.subplots()
mat = ax.matshow(grid, cmap='gray')  # Use a grayscale colormap or any other suitable colormap

# Draw grid lines
ax.set_xticks(np.arange(-.5, grid.shape[1], 1), minor=True)
ax.set_yticks(np.arange(-.5, grid.shape[0], 1), minor=True)
ax.grid(which="minor", color="w", linestyle='-', linewidth=2)
ax.tick_params(which="minor", size=0)

# To avoid drawing border around the whole grid, you can disable the major grid lines
ax.grid(which="major", color="w", linestyle='', linewidth=0)

# Optional: Remove ticks and labels
ax.set_xticks([])
ax.set_yticks([])
ax.set_title("Time 00:00:00:000")

print('Initializing animation')
# Create the animation
ani = animation.FuncAnimation(fig, update, frames=len(times), interval=1000)

print('Saving the animation')
# Save the animation as a video file
ani.save('Gate_animation.mp4', writer='ffmpeg', dpi=150)

# If you want to display the animation inline (in a Jupyter notebook, for example), use:
# from IPython.display import HTML
# HTML(ani.to_html5_video())