# Quick Setup
This document is meant to provide streamlined step-by-step instructions on how to set up a programming environment on your laptop, for those who want to get up and running as quickly as possible. It is a summarized list of the content from Modules 1 and 2.

## Assumptions
In order to streamline the instructions, we will make the following assumptions:
1. You are using **Windows 10** as your main operating system.
2. You are using **Ubuntu** on **Windows Subsystem for Linux (WSL)** as your main programming environment.
3. You are installing the **Python** APIs for the **Gurobi** and **CPLEX** MIP solvers (**C++** API does not require additional steps).
4. You have a basic understanding of how to use the Linux command line. If not, go through the tutorial at <https://linuxsurvival.com/>.

## Things to Note
* If you are using **WSL**, you can access your Windows system from within WSL via the `/mnt` directory. For example, `/mnt/c` corresponds to your `C:\\` drive, `/mnt/d/Users/alice` corresponds to the `D:\\Users\\alice` folder, and so on.
* In the instructions below, X.Y.Z and XXX are placeholders for specific version numbers, which should be substituted appropriately. For example, for Gurobi 8.1 the targz archive is named `gurobi8.1.0_linux64.tar.gz` and the default install directory is `/opt/gurobi810/linux64`; for CPLEX 12.9.0 the binary is named `cplex_studio129.linux-x86-64.bin` and the default install directory is `/opt/ibm/ILOG/CPLEX_Studio129`.

## Instructions
### Install Windows Subsystem for Linux
    1. Go to **Control Panel** > **Programs and Features** > **Turn Windows features on or off**, check **Windows Subsystem for Linux**, and hit **OK**.
    2. Restart your computer when prompted.
### Install and setup Ubuntu
    3. Download and install Ubuntu from Windows Store.
    4. Launch the Ubuntu app; create a new user account and password by following prompts.
    5. Run `sudo apt-get update` to update the package repository.
    6. Run `sudo apt-get upgrade` to upgrade all of your packages.
    7. Run `sudo apt-get autoremove` to remove all outdated packages.
    8. Run `sudo apt-get install python3-pip` to install pip3 on your computer.
### Get Gurobi academic license
    9. Go to <http://www.gurobi.com/registration/academic-license-reg>.
    10. If you don't already have a Gurobi account, register for one. Log in to your account.
    11. Accept the EULA and conditions for an academic license, and press **Request License**.
### Install Gurobi
    12. Go to <http://www.gurobi.com/downloads/gurobi-optimizer> and download the latest 64-bit Linux version of Gurobi (you will need to accept the EULA). This will be in the form of a targz archive (e.g., `gurobiX.Y.Z_linux64.tar.gz`).
    13. If you don't already have an `/opt` directory in your root directory, create one by running `sudo mkdir -m 755 /opt`.
    14. Run a command similar to `sudo mv /mnt/d/Users/alice/Downloads/gurobiX.Y.Z_linux64.tar.gz .` to move the targz archive to the `/opt` directory.
    15. Run `cd /opt; sudo tar -xf gurobiX.Y.Z_linux64.tar.gz` to extract the targz archive in the `/opt` directory.
    16. Run `nano ~/.bashrc` to open your `.bashrc` file.
    17. At the bottom of the file, add the following lines to modify environment variables:
    ```bash
    export GUROBI_HOME="/opt/gurobiXXX/linux64"
    export PATH="${PATH}:${GUROBI_HOME}/bin"
    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${GUROBI_HOME}/lib"
    ```
    18. Restart Ubuntu in order for the modified environment variables to take effect.
### Retrieve Gurobi license
    19. Go to <https://user.gurobi.com/download/licenses/current> and click on the license.
    20. Copy and paste the `grbgetkey` command from the bottom of the license page to your command line terminal, and run it. Note that you must be on CMU's network in order for the command to work.
    21. When prompted, save the license in the default location (e.g., `/home/alice`).
### Set up Gurobi Python API
    22. Navigate to the root Gurobi directory (usually `/opt/gurobiXXX/linux64/`); it should contain a file called `setup.py`.
    23. Run the command `sudo python3 setup.py install` in the current directory.
### Download CPLEX
    24. Go to <https://ibm.onthehub.com>.
    25. Register for an OnTheHub account. You will be asked to verify your email. Note that this is completely separate from your IBM id (if you have one).
    26. Select CPLEX through the store by clicking **Home Page** and navigating to **Students** > **Data & Analytics** > **Software** > **CPLEX** (this can be a bit tricky). Click **Add to Cart**, then click **Check Out**.
    27. Accept the Academic Initiative Program Agreement, and answer the 4 question survey.
    28. At this point, your software request may get (erroneously) rejected. Don't panic; usually the error is resolved in about 10-15 minutes. If not, email them about the error at <https://ibm.onthehub.com/WebStore/Support/ContactUs.aspx>.
    29. Download the appropriate version of the software (Linux x86-64).
### Install CPLEX
    30. Navigate to the location of the binary (e.g., `/mnt/d/Users/alice/Downloads`).
    31. Run `sudo cplex_studioXXX.linux-x86-64.bin` to execute the binary.
    32. Enter `2` to choose the English locale (or a different one if desired).
    33. Enter `1` to accept the license agreement.
    34. Accept the default installation directory, usually `/opt/ibm/ILOG/CPLEX_StudioXXX`.
    35. Begin the installation, then wait for it to finish. Exit the installer after it's done.
    36. Run `nano ~/.bashrc` to open your `.bashrc` file.
    37. At the bottom of the file, add the following lines to modify environment variables:
    ```bash
    export CPLEX_ROOT="/opt/ibm/ILOG/CPLEX_StudioXXX"
    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${CPLEX_ROOT}/opl/bin/x86-64_linux"
    ```
    38. Restart Ubuntu in order for the modified environment variables to take effect.
### Set up CPLEX Python API
    39. Navigate to the root CPLEX directory (usually `/opt/ibm/ILOG/CPLEX_StudioXXX`).
    40. Run `sudo pip3 install python/docplex` to install DOcplex.
    41. Run `sudo pip3 install cplex/python/X.Y/ARCH` to install the legacy Python API, where X.Y is replaced by the version of Python you are using (you can find out with `python3 -V`) and ARCH is replaced by the appropriate name for the architecture.
