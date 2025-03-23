# Gray-Scott Cellular-DEVS

| Linux | Windows | ESP32 | MSP432 |
|:-----:|:-------:|:-----:|:------:|
| ✔️    | ✔️      | ❌    | ❌     |

---

## Introduction

This project implements a 2D **Gray-Scott reaction-diffusion model** using the **Cell-DEVS formalism**. The model captures the nonlinear interaction and diffusion between two chemical species (**u** and **v**) over a grid of cells, resulting in complex spatial patterns such as spots, waves, and stripes.

For a full explanation of the Gray-Scott model and its Cell-DEVS mapping, refer to the paper:  
**John E. Pearson (1993)** – *Complex Patterns in a Simple System*  
[View on ResearchGate](https://www.researchgate.net/publication/6011915_Complex_Patterns_in_a_Simple_System)

---

## Dependencies

This project assumes you have **Cadmium** installed and accessible via the environment variable `$CADMIUM`.

> ✅ **Note:** If you are using the DEVSsim server, you can skip this step — all required dependencies are pre-installed.

---

## Build

To compile the project, simply run:

```bash
source build_sim.sh
```
NOTE: Everytime you run build.sh, the contents of build/ and bin/ will be replaced.

## Execute

To run the simulation, use:

```bash
./bin/gray-scott-cellular config/grayScottConfig.json <simulation_steps>
```

Replace `<simulation_steps>` with the number of steps you wish to simulate.  
For example, to simulate 75 time steps:

```bash
./bin/gray-scott-cellular config/grayScottConfig.json 75
```

You can increase the number (e.g., 100+) for full pattern emergence.

---

## Configuration

Edit the simulation setup in:

```
config/grayScottConfig.json
```

This file defines:
- Grid shape and boundary conditions
- Initial seed positions and concentrations
- Visualization fields and color mappings

---

## Output

Simulation outputs include the evolving concentrations of `u`, `v`, and `v_ratio`, and can be visualized using tools such as DEVS viewer.
