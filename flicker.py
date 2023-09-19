#import matplotlib.pyplot as plt
#
#def plot_square_wave(values):
#    x_points = []
#    y_points = []
#    
#    y = 0  # Initial y value
#    for value in values:
#        # Add points for the vertical line segments
#        x_points.append(value)
#        y_points.append(y)
#        
#        y = 1 - y  # Toggle y-value
#        
#        # Add points for the horizontal line segments
#        x_points.append(value)
#        y_points.append(y)
#        
#    plt.step(x_points, y_points, where='post')
#    plt.xlabel('x')
#    plt.ylabel('y')
#    plt.title('Square Wave Plot')
#    plt.grid(True)
#    plt.show()
#
## Test the function
#values = [0, 1, 2, 3, 4, 5, 6]
#plot_square_wave(values)

#import matplotlib.pyplot as plt
#import random
#
#def plot_random_square_wave(values):
#    x_points = []
#    y_points = []
#
#    y = 0  # Initial y value
#    for value in values:
#        # Add points for the vertical line segments
#        x_points.append(value)
#        y_points.append(y)
#
#        # Randomly toggle y-value
#        y = random.choice([0, 1])
#
#        # Add points for the horizontal line segments
#        x_points.append(value)
#        y_points.append(y)
#
#    plt.step(x_points, y_points, where='post')
#    plt.xlabel('Time')
#    plt.ylabel('State (On/Off)')
#    plt.title('Randomly Flickering Light Simulation')
#    plt.grid(True)
#    plt.show()
#
## Test the function
#values = [i for i in range(20)]
#plot_random_square_wave(values)

import matplotlib.pyplot as plt
import numpy as np
import random

# Define the piecewise function
def piecewise_function(x):
    if 0 <= x <= 5:
        return 1 - x*0.2/5  # Drop 0.2 in 5
    elif 5 < x <= 6:
        return 0.8 - (x - 5)*0.4/1 # drop 0.4 in 1
    elif 6 < x <= 10:
        return 0.4 - (x - 6)*0.2/4 # drop 0.2 in 4
    else:
        return None

# Generate x values
x_values = np.linspace(0, 10, 500)

# Generate y values based on the piecewise function
y_values = [piecewise_function(x) for x in x_values]

plt.plot(x_values, y_values)
plt.xlabel('x')
plt.ylabel('y')
plt.title('Piecewise Function')
plt.grid(True)
plt.show()

duration = 3.0
tsum = 0.0
intervals = []
while True:
    interval = piecewise_function(random.random() * 10)
    if interval <= 0: break
    tsum += interval
    if tsum > duration:
        intervals.append(duration - sum(intervals))
        break
    intervals.append(interval)
print(intervals)

