from distutils.core import setup
from Cython.Build import cythonize

files = [
    "Commands.py",
    "Context.py",
    "Controller.py",
    "Main.py",
    "Model.py",
    "MTM1M3TSController.py",
    "MTM1M3TSEnumerations.py",
    "States.py",
    "Threads.py",
    "ThermalScannerClient.py"
]

setup(name = "MTM1M3TS", ext_modules=cythonize(files))