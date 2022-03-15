# gct535-2022
Code repository for GCT535 Sound Technology for Multimedia (Spring 2022)


# Instruction to set up the course Conda virtual environment 

We will use Python throughout the course. For practice session and homework, we will manage all library packages under a Conda virtual environment named "gct535". As a first step, please install the Python 3 version of Anaconda (https://repo.anaconda.com/). It will provide a cross-platform installation package. 

*** Windows Users: please use Linux running on Windows such as Ubuntu on Windows (https://ubuntu.com/tutorials/ubuntu-on-windows#1-overview) and then run Python on it ***

After you have installed conda, close any open terminals you might have. Then open a new terminal and run the following command:


1. Create a gct535 environment with python 3.8 and install essential packages

   conda create -n gct535 python=3.8 jupyter jupyterlab matplotlib

2. Activate the new environment:

    conda activate gct535

3. Install IPython kernel so we can use this environment in jupyter notebook: 

    python -m ipykernel install --user --name gct535

4. install librosa

    conda install -y -c conda-forge librosa

5. Run Jupyter Notebook 

   jupyter notebook 	

6. Go to the "practice" folder and open one of "*.ipynb" files 
 
7. To deactivate an active environment, use
    
   conda deactivate


