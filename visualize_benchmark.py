import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path


def load_benchmark_data(file_path):
    """Load and preprocess benchmark data from CSV file."""
    df = pd.read_csv(file_path)
    # Group by algorithm, size and calculate mean time
    grouped = df.groupby(['algorithm', 'size'])['time_ms'].mean().reset_index()
    # Pivot the data for plotting
    pivot_df = grouped.pivot(index='size', columns='algorithm', values='time_ms')
    return pivot_df


def create_comparison_plots(data, output_dir=None):
    """Create comparison plots in both linear and logarithmic scales."""
    # Generate color map for all algorithms
    n_algorithms = len(data.columns)
    colors = plt.cm.rainbow(np.linspace(0, 1, n_algorithms))

    # List of different markers to use
    markers = ['o', 's', '^', 'D', 'v', '>', '<', 'p', '*', 'h']

    # Create a figure with two subplots side by side
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))

    # Plot data on both axes
    for idx, algorithm in enumerate(data.columns):
        # Get color and marker (cycling through markers if more algorithms than markers)
        color = colors[idx]
        marker = markers[idx % len(markers)]

        # Linear scale plot
        ax1.plot(data.index, data[algorithm],
                 color=color,
                 marker=marker,
                 label=algorithm,
                 linewidth=1.5,
                 markersize=6)

        # Logarithmic scale plot
        ax2.plot(data.index, data[algorithm],
                 color=color,
                 marker=marker,
                 label=algorithm,
                 linewidth=1.5,
                 markersize=6)

    # Configure linear scale plot
    ax1.set_xlabel('n')
    ax1.set_ylabel('Time in milliseconds')
    ax1.grid(True, which='both', linestyle='--', alpha=0.7)
    ax1.legend()
    ax1.set_title('Linear Scale')

    # Automatically adjust y-axis limits for better visibility
    y_min = data.min().min()
    y_max = data.max().max()
    # Add some padding (10%) to the top and bottom
    y_padding = (y_max - y_min) * 0.1
    ax1.set_ylim(max(0, y_min - y_padding), y_max + y_padding)

    # Add minor ticks for better readability
    ax1.minorticks_on()

    # Format y-axis labels for better readability
    ax1.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: format(int(x), ',')))

    # Configure logarithmic scale plot
    ax2.set_yscale('log')
    ax2.set_xlabel('n')
    ax2.set_ylabel('Time in milliseconds (log scale)')
    ax2.grid(True, which='both', linestyle='--', alpha=0.7)
    ax2.legend()
    ax2.set_title('Logarithmic Scale')

    # Add minor gridlines to log plot
    ax2.grid(True, which='minor', linestyle=':', alpha=0.4)

    # Adjust layout with more space for y-axis labels
    plt.tight_layout(pad=2.0)

    # Save if output directory is specified
    if output_dir:
        output_path = Path(output_dir) / f"benchmark_comparison_{pd.Timestamp.now().strftime('%Y%m%d_%H%M%S')}.png"
        plt.savefig(output_path, dpi=300, bbox_inches='tight')

    return fig


def main():
    """Main function to run the visualization program."""
    # Get the current script directory
    current_dir = Path(__file__).parent

    # Navigate to the benchmark directory
    benchmark_dir = current_dir / "cmake-build-debug" / "benchmark"

    try:
        # List available benchmark files
        benchmark_files = list(benchmark_dir.glob("*.csv"))

        if not benchmark_files:
            print(f"No benchmark files found in {benchmark_dir}")
            return

        # Print available files
        print("\nAvailable benchmark files:")
        for i, file in enumerate(benchmark_files, 1):
            print(f"{i}. {file.name}")

        # Get user selection
        while True:
            try:
                selection = int(input("\nSelect a file number to visualize (or 0 to exit): "))
                if selection == 0:
                    return
                if 1 <= selection <= len(benchmark_files):
                    selected_file = benchmark_files[selection - 1]
                    break
                print("Invalid selection. Please try again.")
            except ValueError:
                print("Please enter a valid number.")

        # Load and process data
        data = load_benchmark_data(selected_file)

        # Create output directory next to the benchmark directory
        output_dir = benchmark_dir.parent / "benchmark_plots"
        output_dir.mkdir(exist_ok=True)

        # Generate and display plots
        fig = create_comparison_plots(data, output_dir)
        plt.show()

        print(f"\nPlots have been saved to {output_dir}")

    except Exception as e:
        print(f"Error: {e}")
        print(f"Attempted to access benchmark directory: {benchmark_dir}")


if __name__ == "__main__":
    main()