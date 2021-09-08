import subprocess
import time

def forward_time(time, handle):
    print("Forwarding time by {}s".format(time))
    handle.execute_script("window.__forwardTime({})".format(time))

def clear_database(handle):
    print("Clearing database")
    time.sleep(4)
    #handle.execute_script("window.__clearDb()")
    time.sleep(5)