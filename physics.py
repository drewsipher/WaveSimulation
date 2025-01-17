import numpy as np

class WaveSimulation:
    
    def __init__(self, grid, medium_properties, wave_params):
        self.grid = grid
        self.base_speed = medium_properties['speed_of_sound']
        self.time_step = wave_params['time_step']
        self.damping = wave_params.get('damping', 0.9999)  # Base damping factor
        
        # Calculate speed grid based on density (e.g., inverse relation)
        self.speed = self.base_speed / self.grid.density
        self.coeff = (self.speed ** 2) * (self.time_step ** 2)
        
        # Define damping layer properties
        self.damping_thickness = 25  # Number of cells for damping layers
        self.create_damping_mask()
    
    def create_damping_mask(self):
        size = self.grid.size
        damping_thickness = self.damping_thickness
        damping_strength = 0.9  # Damping factor within damping layers
        
        self.damping_mask = np.ones((size, size))
        
        # Top and Bottom Damping
        for i in range(damping_thickness):
            factor = damping_strength + (1.0 - damping_strength) * (i / damping_thickness)
            self.damping_mask[i, :] *= factor  # Top
            self.damping_mask[-i-1, :] *= factor  # Bottom
        
        # Left and Right Damping
        for i in range(damping_thickness):
            factor = damping_strength + (1.0 - damping_strength) * (i / damping_thickness)
            self.damping_mask[:, i] *= factor  # Left
            self.damping_mask[:, -i-1] *= factor  # Right
    
    def update(self):
        # Compute Laplacian without wrapping
        laplacian = self.compute_laplacian(self.grid.grid)
        
        # Wave equation update
        new_grid = 2 * self.grid.grid - self.grid.prev_grid + self.coeff * laplacian
        
        # Apply base damping
        new_grid *= self.damping
        
        # Apply damping mask to absorb waves at boundaries
        new_grid *= self.damping_mask
        
        # Update grids
        self.grid.prev_grid = self.grid.grid.copy()
        self.grid.grid = new_grid.copy()

    def compute_laplacian(self, grid):
        # Initialize Laplacian with zeros
        laplacian = np.zeros_like(grid)
        
        # Central region (exclude boundaries)
        laplacian[1:-1,1:-1] = (
            grid[0:-2,1:-1] + grid[2:,1:-1] +
            grid[1:-1,0:-2] + grid[1:-1,2:] -
            4 * grid[1:-1,1:-1]
        )
        
        return laplacian