import numpy as np

class SimulationGrid:
    def __init__(self, settings):
        self.size = settings['size']
        self.spacing = settings['spacing']
        self.grid = np.zeros((self.size, self.size))
        self.prev_grid = np.zeros_like(self.grid)

    def update(self):
        np.copyto(self.prev_grid, self.grid)