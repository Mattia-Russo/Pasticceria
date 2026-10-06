# Pasticceria

Pasticceria is a C-based simulation of a bakery production and logistics system. The program manages a set of recipes, tracks ingredients in stock with expiration dates, accepts or rejects customer orders, and schedules deliveries based on capacity and time windows.

The project appears to be an algorithmic/operational challenge implementation focused on efficient data structures, inventory management, and order scheduling.

## Project purpose

The program reads commands from standard input and simulates the full lifecycle of bakery operations:

- adding and removing recipes
- refilling the warehouse with ingredients
- checking if an order can be fulfilled
- accepting or delaying orders when ingredients are missing
- delivering ready orders when a truck arrives

## Main files

- `Bakery_veloce3.c` — main implementation
- `Makefile` — compiler flags configuration
- `Open/` — sample input and expected output files used for testing
- `Generatori/` — generator utilities and test artifacts
- `callgrind/` and `massif/` — profiling data produced with `valgrind`
- PDF files in the root — project documentation and presentation material

## Build

The project is implemented in C and can be compiled directly with GCC:

```bash
gcc -Wall -Werror -std=gnu11 -g3 -O2 -fsanitize=address Bakery_veloce3.c -lm -o Bakery_veloce3
```

Then run it with an input file or through stdin:

```bash
./Bakery_veloce3 < input.txt
```

## Expected input format

The program expects the first line to contain:

```text
period capacity
```

Where:

- `period` is the delivery interval
- `capacity` is the truck capacity

It then processes a sequence of commands such as:

- `aggiungi_ricetta nome_ricetta ingredient1 quantity1 ingredient2 quantity2 ...`
- `rimuovi_ricetta nome_ricetta`
- `rifornimento ingredient quantity expiration`
- `ordine nome_ricetta quantity`

The application prints status messages such as:

- `aggiunta`
- `ignorato`
- `rimossa`
- `non presente`
- `accettato`
- `rifiutato`
- `camioncino vuoto`

## Example workflow

```text
3 10
aggiungi_ricetta panino farina 5 uova 2
rifornimento farina 20 10
rifornimento uova 10 15
ordine panino 2
```

This is a simplified example of how the bakery system would manage recipes and inventory.

## Technical notes

This repository includes performance-analysis artifacts, which suggests the project was evaluated for optimization and memory behavior during development. The presence of `callgrind` and `massif` output files indicates the code was tested under profiling tools to measure execution time and memory usage.

## Repository status

The repository contains a mostly C-based implementation, with a few support directories and reference artifacts for testing and documentation.

## License

No explicit license file was found in the repository metadata. If you intend to redistribute or publish this project, consider adding a suitable open-source license such as MIT or GPL.

## Contributions

This project appears to be a personal academic or project-based implementation. If you want to extend it, the typical workflow would be:

1. fork the repository
2. create a feature branch
3. modify the C implementation
4. test with the provided input files or custom scenarios
5. submit a pull request

