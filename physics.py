import numpy as np

class WaveSimulation:
    """
    2D Wave equation solver using finite difference method.
    
    Simulates wave propagation in a 2D medium using the wave equation:
    ∂²u/∂t² = c²(∂²u/∂x² + ∂²u/∂y²)
    
    Implements numerical solution using finite difference method with optional damping.
    """
    def __init__(self, grid, medium_properties, wave_params):
        """
        Initialize wave simulation parameters.
        
        Args:
            grid: Grid object containing current and previous wave states
            medium_properties: Dictionary with medium properties containing 'speed_of_sound'
            wave_params: Dictionary with 'time_step' and optional 'damping' parameter
        """
        self.grid = grid
        self.speed = medium_properties['speed_of_sound']
        self.time_step = wave_params['time_step']
        self.coeff = (self.speed ** 2) * (self.time_step ** 2)
        self.damping = wave_params.get('damping', 0.99)  # Damping factor, default to 0.99

    def update(self):
        """
        Update wave field for one time step using finite difference method.
        
        Applies wave equation update, damping, and reflective boundary conditions.
        """
        # Compute wave equation update (simplified finite difference)
        laplacian = self.compute_laplacian(self.grid.grid)
        new_grid = 2 * self.grid.grid - self.grid.prev_grid + self.coeff * laplacian
        
        # Apply damping
        new_grid *= self.damping
        
        # Apply reflective boundary conditions
        new_grid[0, :] = new_grid[1, :]  # Top boundary
        new_grid[-1, :] = new_grid[-2, :]  # Bottom boundary
        new_grid[:, 0] = new_grid[:, 1]  # Left boundary
        new_grid[:, -1] = new_grid[:, -2]  # Right boundary
        
        # Update grids
        self.grid.prev_grid = self.grid.grid
        self.grid.grid = new_grid

    def compute_laplacian(self, grid):
         """
        Compute discrete Laplacian using 5-point stencil method.
        
        Args:
            grid: Current wave field state array
            
        Returns:
            NDArray[np.float64]: Computed Laplacian of the wave field
        """
        return np.roll(grid, 1, axis=0) + np.roll(grid, -1, axis=0) + \
               np.roll(grid, 1, axis=1) + np.roll(grid, -1, axis=1) - 4 * grid