import numpy as np

class SimulationGrid:
    def __init__(self, grid_settings, density_settings):
        self.size = grid_settings['size']
        self.spacing = grid_settings['spacing']
        self.grid = np.zeros((self.size, self.size))
        self.prev_grid = np.zeros_like(self.grid)

        # Initialize density grid
        self.density = np.full((self.size, self.size), density_settings['default_density'])
        
        # Apply density variations if any
        for variation in density_settings.get('density_variations', []):
            x_start, x_end = variation['x_range']
            y_start, y_end = variation['y_range']
            self.density[y_start:y_end, x_start:x_end] = variation['density']

    def update(self):
        np.copyto(self.prev_grid, self.grid)