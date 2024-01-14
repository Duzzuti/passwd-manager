import os
import sys
import getpass

DELETE_DEC_FILE = True
CONTAINER_ID = 
CONTAINER_CLEAR_PATH = "/home/duzzuti/passwd-manager/extern/clear_files.py"
CONTAINER_OUTPUT_PATH = "/home/duzzuti/passwd-manager/data/outputs.txt"
CONTAINER_DATA_PATH = "/home/duzzuti/passwd-manager/data/files/"
CONTAINER_SRC_PATH = "/home/duzzuti/passwd-manager/build/src/pman"

# get password
# start container
# copying to container (docker cp)
# run skript in container (docker exec)
# copy from container (rename) (docker cp)
# stop container

def get_path():
    path = os.getcwd() + "/" + sys.argv[1]
    if os.path.exists(path):
        return path
    else:
        return None

if(os.path.exists(os.getcwd()+"/outputs.txt")):
    print("Error occured, file already exists")
    exit(1)

# get password
if(len(sys.argv) <= 1):
    print("No path given")
    exit(1)
else:
    path = get_path()
    if(path == None):
        print("Path does not exist")
        exit(1)

    if(len(sys.argv) == 2):
        password = getpass.getpass("Enter file password: ")
    else:
        password = sys.argv[2]

# start container
os.system("docker start "+CONTAINER_ID)
# copying to container (docker cp)
os.system("docker cp "+path+" "+CONTAINER_ID+":"+CONTAINER_DATA_PATH)
# run skript in container (docker exec)
os.system("docker exec "+CONTAINER_ID+" "+CONTAINER_SRC_PATH+" "+CONTAINER_DATA_PATH+sys.argv[1]+" "+password)
# get name of result file
os.system("docker cp "+CONTAINER_ID+":"+CONTAINER_OUTPUT_PATH+" "+os.getcwd()+"/outputs.txt")
with open(os.getcwd()+"/outputs.txt") as f:
    for line in f:
        pass
    decName = line
    if(decName.count("/") > 0):
        decName = decName.split("/")[-1].strip()
os.remove(os.getcwd()+"/outputs.txt")
# copy from container (rename) (docker cp)
if(decName.split(".")[-1] == "enc"):
    os.system("docker cp "+CONTAINER_ID+":"+CONTAINER_DATA_PATH+decName+" "+os.getcwd()+"/"+sys.argv[1].split(".")[0]+".enc")
else:
    os.system("docker cp "+CONTAINER_ID+":"+CONTAINER_DATA_PATH+decName+" "+os.getcwd()+"/")
# remove file from container
os.system("docker exec "+CONTAINER_ID+" /bin/python3 "+CONTAINER_CLEAR_PATH)
if(DELETE_DEC_FILE):
    os.remove(get_path())
# stop container
os.system("docker stop "+CONTAINER_ID)