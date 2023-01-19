# Cellular Automata 3D
Implementation of 3d cellular automata in C++20 as described in [this article](https://softologyblog.wordpress.com/2019/12/28/3d-cellular-automata-3/). A brief explanation of the rules is further down below.

![Application Overview](docs/Application.gif)

## Dependencies
The repo contains VS2022 solution and project files and uses C++20.
Dependencies are included in `external/`
- [Raylib](https://github.com/raysan5/raylib) for 3d rendering + window management
- [Raygui](https://github.com/raysan5/raygui) for UI rendering
- [magic_enum](https://github.com/Neargye/magic_enum) to make it easier to use enums for UI settings

## Controls
![Control Buttons](docs/Controls.png)

Controls can be found in the top center of the window.
- **RESET** stops the current simulation, applies all settings from the UI and initializes the grid to the initial values
- **STEP** simulates a single step
- **PLAY**/**PAUSE** starts of pauses the current simulation
- Mouse wheel to zoom in/out

## Settings
![Settings](docs/Settings.png)

Some settings like the render and color settings can be changed after the simulation was prepared or is running. Others such as size, states or rules will be highlighted and can be applied by pressing the **RESET** button.

| SETTING | DESCRIPTION | VALUES |
| ------- | ----------- | ------ |
| **Size** | The amount of cells on each axis | 5-100 |
| **Render Mode** | How each non empty cell is displayed | Quad, Cube, Point |
| **Color Mode** | How the color of each non empty cell is determined | Radius, XYZ, State |
| **Gradient** | The gradient that will be used to colorize the cells | Random, Random_2, Random_3, Random_4, Random_5, Grayscale, Grayscale_Reverse, Hue, Hue_Reverse |
| **Fill Shape** | In which shape the initial cells are filled | Cube, Sphere |
| **Fill Diameter** | The diameter of the **Fill Shape** that will be used to fill the initial cells | 1-**Size** |
| **Fill Prob** | The probability that a cell in the **Fill Shape** will be filled | 0-100% |
| **Wrap Around** | Determines whether the neighbours on the opposite side of the simulation cube will be counted or not | Yes, No |
| **Steps/s** | The amount of automatic simulation steps to run each second, if the play button was pressed | 0-60 |
| **Neighbours** | The method to calculate neighbours | Moore *= 26 possible neighbours*, VonNeumann *= 6 possible neighbours* |
| **States** | The amount of states each cell can have. 2 = on/off, 5 = 4 visible states + off | 2-64 |
| **Survive Rule** | Rule for cell survival (see below for more info) | List of comma separated numbers or ranges *(1,2,3-5,7,10-12)* |
| **Spawn Rule** | Rule for cell spawning (see below for more info) | List of comma separated numbers or ranges *(1,2,3-5,7,10-12)* |

## Rules
For a more detailed explanation, I recommend [this article](https://softologyblog.wordpress.com/2019/12/28/3d-cellular-automata-3/).

- **Neighbour mode**
    - **Moore** = all 26 adjacent cells (including diagonals) are considered
    - **Von Neumann** = the 6 adjacent cells with a side touching the target cell count es neighbour
- **States**
    - Each cell has a `state in [0, states)`
    - `0` means the cell is empty and invisible
    - `state > 0` means the cell is visible
    - `state == states - 1` means the cell counts as neighbour to other cells
    - A cell with `state < states - 1` will decay every simulation step until the `state` is `0` and the cell empty/invisible
- **Survive Rule**
    - The survive rule is a `list` of values in `[0, states)`
    - Every simulation step, the `neighbours` of each cell are counted
    - If a cell is in state `states - 1` and `neighbours` is in the `list`, the cell survives...
    - ... otherwise the `state` is decremented and decays every simulation step until the `state` is `0`
- **Spawn Rule**
    - The spawn rule is a `list` of values in `[0, states)`
    - Every simulation step, the `neighbours` of each cell are counted
    - If a cell has a `state` of `0` *(it's empty)* and `neighbours` is in the `list`, the cell `state` is set to `states - 1` to spawn a new cell
