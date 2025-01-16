import numpy as np

class WaveSimulation:
    def __init__(self, grid, medium_properties, wave_params):
        self.grid = grid
        self.speed = medium_properties['speed_of_sound']
        self.time_step = wave_params['time_step']
        self.frequency = wave_params['frequency']
        self.wavelength = wave_params['wavelength']

    def update(self):
        # Compute wave equation update (simplified finite difference)
        laplacian = self.compute_laplacian(self.grid.grid)
        new_values = 2 * self.grid.grid - self.grid.prev_grid + (self.speed ** 2) * self.time_step ** 2 * laplacian
        
       # Apply reflective boundary conditions
        new_values[0, :] = new_values[1, :]  # Top boundary
        new_values[-1, :] = new_values[-2, :]  # Bottom boundary
        new_values[:, 0] = new_values[:, 1]  # Left boundary
        new_values[:, -1] = new_values[:, -2]  # Right boundary
        
        self.grid.update(new_values)

    def compute_laplacian(self, grid):
        return np.roll(grid, 1, axis=0) + np.roll(grid, -1, axis=0) + \
               np.roll(grid, 1, axis=1) + np.roll(grid, -1, axis=1) - 4 * grid