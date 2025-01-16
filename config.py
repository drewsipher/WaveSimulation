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


NUM_TIME_STEPS = 1000
