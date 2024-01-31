import os, shutil

path = os.path.dirname("/home/duzzuti/passwd-manager/data/files/")
shutil.rmtree(path)
os.mkdir(path)

# delete last line of output file if it is not encrypted file name
with open("/home/duzzuti/passwd-manager/data/outputs.txt") as f:
    lines = f.readlines()
    writeback = lines[:-1]
    decName = lines[-1]
    if(decName.count("/") > 0):
        decName = decName.split("/")[-1].strip()

if(decName.split(".")[-1] != "enc"):
    with open("/home/duzzuti/passwd-manager/data/outputs.txt", "w") as f:
        f.writelines(writeback)