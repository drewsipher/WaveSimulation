import numpy as np

class WaveSimulation:
    def __init__(self, grid, medium_properties, wave_params):
        self.grid = grid
        self.speed = medium_properties['speed_of_sound']
        self.time_step = wave_params['time_step']
        self.coeff = (self.speed ** 2) * (self.time_step ** 2)

    def update(self):
        # Compute wave equation update (simplified finite difference)
        laplacian = self.compute_laplacian(self.grid.grid)
        new_grid = 2 * self.grid.grid - self.grid.prev_grid + self.coeff * laplacian
        
        # Apply reflective boundary conditions
        new_grid[0, :] = new_grid[1, :]  # Top boundary
        new_grid[-1, :] = new_grid[-2, :]  # Bottom boundary
        new_grid[:, 0] = new_grid[:, 1]  # Left boundary
        new_grid[:, -1] = new_grid[:, -2]  # Right boundary
        
        # Update grids
        self.grid.prev_grid = self.grid.grid
        self.grid.grid = new_grid

    def compute_laplacian(self, grid):
        return np.roll(grid, 1, axis=0) + np.roll(grid, -1, axis=0) + \
               np.roll(grid, 1, axis=1) + np.roll(grid, -1, axis=1) - 4 * grid