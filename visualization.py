import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore
import numpy as np

class Visualizer:
    def __init__(self, grid):
        self.grid = grid
        self.app = QtWidgets.QApplication([])
        self.win = pg.GraphicsLayoutWidget(show=True)
        self.plot = self.win.addPlot()
        self.plot.setAspectLocked(True)  # Lock the aspect ratio to be square
        self.img = pg.ImageItem()
        self.plot.addItem(self.img)
        self.closed = False
        self.win.closeEvent = self.on_close
        self.plot.scene().sigMouseClicked.connect(self.on_click)
        self.audio_sources = []
        self.barriers = []

        # Disable mouse panning and zooming
        self.plot.setMouseEnabled(x=False, y=False)

        # Define fixed min and max values for the wave amplitude
        self.min_value = -1.0
        self.max_value = 1.0

        # Normalize density for tinting
        self.normalized_density = self.grid.density / (self.grid.density.max())

    def update(self):
        
        # Create a base color map for wave amplitude
        wave_image = self.grid.grid.T
        wave_image_normalized = (wave_image - self.min_value) / (self.max_value - self.min_value)
        wave_rgb = pg.colormap.get('viridis').map(wave_image_normalized, mode='float')

        # Apply tint based on density
        tint = np.stack([self.normalized_density]*4, axis=2)  # Create RGB tint
        
        tinted_wave = wave_rgb * tint

        # Convert to 8-bit image
        tinted_wave_8bit = (tinted_wave * 255).astype(np.uint8)

        # Update the image
        self.img.setImage(tinted_wave_8bit, levels=(0, 255))
        QtWidgets.QApplication.processEvents()  # Process events to keep the UI responsive

    def show(self):
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(16)  # Start the timer with no delay

    def close(self):
        self.closed = True
        self.win.close()

    def on_close(self, event):
        self.closed = True

    def on_click(self, event):
        pos = event.scenePos()
        mouse_point = self.plot.vb.mapSceneToView(pos)
        x, y = int(mouse_point.x()), int(mouse_point.y())
        if event.button() == QtCore.Qt.LeftButton:  # Left click to add audio source
            self.audio_sources.append((x, y))
            self.grid.grid[y, x] = 1  # Example value for audio source
        elif event.button() == QtCore.Qt.RightButton:  # Right click to add barrier
            self.barriers.append((x, y))
            self.grid.grid[y, x] = -1  # Example value for barrier