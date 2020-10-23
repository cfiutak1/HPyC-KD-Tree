import random

random_funcs = [random.uniform, random.gauss, random.betavariate, random.expovariate]
random_func_params = [(-1000, 1000), (0, 1000), (30, 20), (0.01,)]
distribution_name_map = ["uniform", "centered_uniform", "beta", "exponential"]