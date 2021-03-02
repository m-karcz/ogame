import subprocess

def forward_time(time, handle):
    print("Forwarding time by {}s".format(time))
    handle.execute_script("window.__forwardTime({})".format(time));