#turns a hex string into a character string (ascii)
#copys the string in your clipboard
from tkinter import Tk
def main():
    hex = "e7cf3ef4f17c3999a94f2c6f612e8a888e5b1026878e4e19398b23bd38ec221a"    #insert hex here
    print(len(hex))
    hex = hex.upper()
    hexchars = "0123456789ABCDEF"
    chars = []
    if len(hex) % 2 != 0:
        print("invalid hex")
        return
    for i in range(0, len(hex), 2):
        chars.append(hexchars.index(hex[i])*16 + hexchars.index(hex[i+1]))

    #convert from acsii to chars
    print(chars)
    chars = list(map(lambda x: chr(x), chars))
    r = Tk()
    r.withdraw()
    r.clipboard_append("".join(chars))
    r.update() 
    r.destroy()

if __name__ == "__main__":
    main()