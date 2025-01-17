from physics import WaveSimulation
from grid import SimulationGrid
from visualization import Visualizer
import config

def main():
    # Load configuration
    print("Loading configuration...")
    medium_properties = config.MEDIUM_PROPERTIES
    grid_settings = config.GRID_SETTINGS
    density_settings = config.DENSITY_SETTINGS
    wave_params = config.WAVE_PARAMETERS

    # Initialize simulation grid
    print("Initializing simulation grid...")
    grid = SimulationGrid(grid_settings,density_settings)
    print("Initial grid values:")
    print(grid.grid)

    # Initialize wave simulation
    print("Initializing wave simulation...")
    simulation = WaveSimulation(grid, medium_properties, wave_params)

    # Initialize visualizer
    print("Initializing visualizer...")
    visualizer = Visualizer(grid)
    visualizer.show()

    # Run simulation
    print("Starting simulation loop...")
    while not visualizer.closed:
        simulation.update()
        visualizer.update()
        
    visualizer.close()
    print("Simulation ended.")

if __name__ == "__main__":
    main()