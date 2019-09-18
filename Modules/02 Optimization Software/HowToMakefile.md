# How to Use Makefile\_gurobi and Makefile\_cplex

This module contains two minimal makefiles, **Makefile\_gurobi** and **Makefile\_cplex**, which can be used to compile the examples provided with Gurobi/CPLEX on Linux. To use them, use the following steps:

1. Download the makefile and copy it to the same directory containing your source file (e.g., **example.cpp**).
2. Rename the file to **Makefile**.
3. In **Makefile**, edit the line specifying MODEL\_NAME to match your source file (e.g., `MODEL_NAME = example` if your source file is called **example.cpp**).
  * If you are using Gurobi, double check the line specifying CPPLIB to ensure you are linking the correct libraries. See <http://www.gurobi.com/documentation/8.1/quickstart_linux/cpp_building_and_running_t.html> and <http://www.gurobi.com/products/supported-platforms> for details.
4. Run the `make` command. This will generate the binary in the same directory. If you have problems, check that your environment variables are set correctly (in particular, make sure `GUROBI_HOME` and `CPLEX_ROOT` are set to the root directory of Gurobi and CPLEX, respectively; you can find out the value of your environment variables with the command `env`).
