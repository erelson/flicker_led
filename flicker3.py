import random
import threading
import time

signal_state = False
flicker_duration = 3.0
max_timeout = 10.0

light_state = 1  # 1 = on, 0 = off

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

def get_intervals():
    # Returns a monotonically increasing list of times to alternate the light state
    duration = flicker_duration
    tsum = 0.0
    intervals = []
    while True:
        interval = piecewise_function(random.random() * 10)
        if interval <= 0: break
        tsum += interval
        if tsum > duration:
            #intervals.append(duration - sum(intervals))
            intervals.append(duration - intervals[-1])
            break
        #intervals.append(interval)
        intervals.append(tsum)
    # TODO do I reverse the list here?
    intervals.reverse()
    return intervals


# Mock our light functions
def turn_on_light():
    print("on")

def turn_off_light():
    print("off")

def button_pressed():
    return signal_state


def main():
    global signal_state, light_state
    print("HELLO")
    timeout = max_timeout
    last_loop_start = time.time()
    intervals = []
    while True:
        loop_start = time.time()
        # Increase timeout while we hvae had a recent "event"
        if timeout < max_timeout:
            prev_loop_dur = loop_start - last_loop_start
            timeout += prev_loop_dur
            #print(timeout)
            #time.sleep(1)
        last_loop_start = loop_start

        # New event triggered, but only if we're not processing another recent one
        if timeout >= max_timeout:
            if button_pressed():  # will define elsewhere
                print("Button pressed!")
                timeout = 0
                signal_state = False
                
                # Get light flickering intervals (applies for first 3 seconds
                intervals = get_intervals()
                print(intervals)
                int_idx = len(intervals) - 1

        # If we have intervals, then we're flickering   
        if intervals:
            # Get the next interval
            interval = intervals[int_idx]
            # As we hit each interval after timeout was reset to max_timeout, toggle the light
            # If we're on the last interval, turn on the light
            if int_idx == 0:
                intervals = []
                turn_on_light()
            elif interval < timeout:
                print(timeout)
                # Toggle the light
                if light_state == 1:
                    turn_off_light()
                    light_state = 0
                else:
                    turn_on_light()
                    light_state = 1
                int_idx -= 1
            #if interval + timeout < max_timeout:


if __name__ == "__main__":
    print("WTF")
    threadproc = threading.Thread(target=main)
    threadproc.daemon = True
    threadproc.start()
    while True:
        input("Press enter to start flickering")
        print(threadproc.is_alive())
        signal_state = True
        time.sleep(3)
