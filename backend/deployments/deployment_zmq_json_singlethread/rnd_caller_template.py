import subprocess

def forward_time(time, handle = None):
    print("Forwarding time by {}s".format(time))
    subprocess.call(["RND_CALLER_PATH", "forwardTime", str(time)])

def clear_database(handle = None):
    print("Clearing database")
    subprocess.call(["RND_CALLER_PATH", "clearDb"])