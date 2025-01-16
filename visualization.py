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

        # Define fixed min and max values for the color scale
        self.min_value = -1.0
        self.max_value = 1.0

    def update(self):
        self.img.setImage(self.grid.grid.T, levels=(self.min_value, self.max_value))
        QtWidgets.QApplication.processEvents()  # Process events to keep the UI responsive

    def show(self):
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(0)  # Start the timer with no delay


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