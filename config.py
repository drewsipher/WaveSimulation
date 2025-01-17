# config.py
# Configuration settings for the simulation
MEDIUM_PROPERTIES = {
    'speed_of_sound': 343  # meters per second
}

GRID_SETTINGS = {
    'size': 500,
    'spacing': 0.01  # meters
}

WAVE_PARAMETERS = {
    'time_step': 0.0005,  # seconds
    'frequency': 440,    # Hz
    'wavelength': 0.78,   # meters
    'damping': 0.9999  # Add damping factor here
}

# Add density configurations if needed
DENSITY_SETTINGS = {
    'default_density': 1.0,
    'density_variations': [
        {'x_range': (100, 200), 'y_range': (100, 200), 'density': 2.0},
    ]
}