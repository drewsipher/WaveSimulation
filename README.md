# 🌊 WaveSimulation

Welcome to **WaveSimulation**! This project is a captivating Python-based simulation that visualizes wave propagation using the power of PyQtGraph and PyQt5. Dive into the mesmerizing world of waves and see how they interact with barriers and audio sources in real-time!

## 🚀 Features

- **Real-Time Wave Simulation**: Watch waves propagate, reflect, and interact with barriers and audio sources.
- **Interactive Visualization**: Click to add audio sources or barriers and see the immediate impact on wave behavior.
- **Configurable Parameters**: Easily adjust simulation parameters like grid size, wave frequency, and speed of sound.
- **High Performance**: Leveraging NumPy for efficient computations and PyQtGraph for fast, interactive plotting.

## 🛠️ Installation

To get started with WaveSimulation, follow these steps:

1. **Clone the repository**:
   ```sh
   git clone git@github.com:drewsipher/WaveSimulation.git
   cd WaveSimulation
   ```
Create and activate a virtual environment:
    ```sh
    python -m venv .venv
    .venv\Scripts\activate  # On Windows
    source .venv/bin/activate  # On macOS/Linux
    ```
Install the dependencies:
    ```sh
    pip install -r requirements.txt
    ```
🎮 Usage
Run the simulation with the following command:
    ```sh
    python main.py
    ```

Interactive Controls
Left Click: Add an audio source at the clicked location.

📁 Project Structure
config.py: Configuration settings for the simulation.
grid.py: Manages the simulation grid.
physics.py: Implements the wave propagation logic.
visualization.py: Provides real-time visualization using PyQtGraph.
main.py: The entry point of the program.

📜 License
This project is licensed under the MIT License. See the LICENSE file for details.

🤝 Contributing
Contributions are welcome! Feel free to open issues or submit pull requests.

🌟 Acknowledgements
NumPy
PyQtGraph
PyQt5

📬 Contact
For any questions or suggestions, please don't reach out. I don't care... in the nicest possible way.

Enjoy exploring the fascinating world of wave simulations! 🌊✨