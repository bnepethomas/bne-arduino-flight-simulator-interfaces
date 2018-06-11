import os

try:

    
    print(str(os.getlogin()))
    print(str(os.getpid()))
    print(str(os.uname()))
    
except Exception as other:
    print(other)
