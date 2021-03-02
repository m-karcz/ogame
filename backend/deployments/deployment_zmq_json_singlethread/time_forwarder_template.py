import subprocess

def forward_time(time, handle = None):
    print("Forwarding time by {}s".format(time))
    subprocess.call(["TIME_FORWARDER_APP_PATH", str(time)])